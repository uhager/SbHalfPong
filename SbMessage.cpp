/*! \file SbMessage.cpp
  part of SDL2-basic
  author: Ulrike Hager
 */


#include <stdexcept>
#include <sstream>
#include <iomanip>

#include "SbTexture.h"
#include "SbWindow.h"

#include "SbMessage.h"


SbMessage::SbMessage(SbRectangle box, const SbDimension* ref)
  : SbObject(box, ref)
{
}



void
SbMessage::set_text(std::string message)
{
  if ( !font_)
    throw std::runtime_error( "[SbMessage::set_text] no font. Call set_font before setting the text." );

  texture_->from_text( window->renderer(), message, font_.get(), color_);
}



SbFpsDisplay::SbFpsDisplay(std::shared_ptr<TTF_Font> font, SbRectangle box, const SbDimension* ref)
  : SbMessage(box, ref)
{
  name_ = "fps";
  set_font(font);
  start_timer();
}



void
SbFpsDisplay::handle_event(const SDL_Event& event)
{
  if ( event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f ) {
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_LALT]){
      render_me_ = !render_me_;
    }
  }
}


void
SbFpsDisplay::set_number_frames( uint32_t n )
{
  n_frames_ = n;
  while ( times_.size() > n_frames_ ) {
    sum_ -= times_.front();
    times_.pop_front();
  }
}


void
SbFpsDisplay::update()
{
  times_.push_back( time() );
  start_timer();
  sum_ += times_.back();
  if ( times_.size() > n_frames_ ) {
    sum_ -= times_.front();
    times_.pop_front();
  }
  double average = 1000 * times_.size() / sum_ ;
  set_text( std::to_string( int(average) ) + " fps" );
}


/*! SbHighScore implementation
 */
SbHighScore::SbHighScore(std::shared_ptr<TTF_Font> font, SbRectangle box, const SbDimension* ref)
  : SbMessage(box, ref)
{
  font_ = font;
  name_ = "gameover" ;  //!< same name to render only when game over.
  read_highscores();
  //  highscores_.push_back(0);
}


bool
SbHighScore::check_highscore(uint32_t score, bool(SbHighScore::*fctn)(uint32_t, uint32_t), uint32_t level, double multiplier)
{
  bool result = false;
  if ( level >= highscores_.size() ) {
    while ( level > highscores_.size() )
      highscores_.push_back(0);
    highscores_.push_back( score );
    result = true;
  }
  else {
    if ( (this->*fctn)(score, level) ) {  // highscore is actually time, faster is better
      highscores_.at(level) = score;
      result = true;
    }
  }
  std::ostringstream strstr;
  if (result) {
    write_highscores();
    strstr << "*** New record: " << std::fixed << std::setprecision(precision_) << score * multiplier << postfix << " ***" ; 
  }
  else {
    strstr << prefix << " " << std::fixed << std::setprecision(precision_) << score * multiplier << postfix << " -- Record: " << highscores_.at(level) * multiplier << postfix ;
  }
  set_text( strstr.str() );
  return result;
}


bool
SbHighScore::lower(uint32_t score, uint32_t level)
{
  return ( highscores_.at(level) == 0 || score < highscores_.at(level) );
}


bool
SbHighScore::higher(uint32_t score, uint32_t level)
{
  return (  score > highscores_.at(level) );
}



void
SbHighScore::write_highscores( )
{
  SDL_RWops* file = SDL_RWFromFile( savefile.c_str() , "w+b" );
  if ( !file ) {
    throw std::runtime_error("[SbHighScore::write_highscores] Error: Couldn't open file " + savefile + ":\n" + SDL_GetError() );
  }
  uint32_t size = highscores_.size();
  SDL_RWwrite( file, &size, sizeof(uint32_t), 1 );
  for ( auto score: highscores_ ) {
    SDL_RWwrite( file, &score, sizeof(uint32_t), 1 );
  }
  SDL_RWclose( file );
  // std::cout << "[SbHighScore::write_highscores] " ;
  // std::ostream_iterator<uint32_t> iter(std::cout);
  // std::copy(highscores_.begin(), highscores_.end(), iter);
}


std::vector<uint32_t>
SbHighScore::read_highscores( )
{
  SDL_RWops* file = SDL_RWFromFile( savefile.c_str() , "rb" );
  if ( file ) {
    uint32_t max = 0;
    SDL_RWread( file, &max, sizeof(uint32_t), 1 );
    uint32_t scores[max];
    SDL_RWread( file, &scores, sizeof(uint32_t), max );
    highscores_.assign( &scores[0], &scores[0]+max );
    SDL_RWclose( file );
  }
  // std::cout << "[SbHighScore::read_highscores] " ;
  // std::ostream_iterator<uint32_t> iter(std::cout);
  // std::copy(highscores_.begin(), highscores_.end(), iter);
  return highscores_;
}


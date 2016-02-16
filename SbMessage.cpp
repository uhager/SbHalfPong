/*! \file SbMessage.cpp
  part of SDL2-basic
  author: Ulrike Hager
 */


#include <stdexcept>

#include "SbTexture.h"
#include "SbWindow.h"

#include "SbMessage.h"

SbMessage::SbMessage(double x, double y, double width, double height)
  : SbObject(x,y,width,height)
{
}



void
SbMessage::set_text(std::string message)
{
  if ( !font_)
    throw std::runtime_error( "[SbMessage::set_text] no font. Call set_font before setting the text." );

  texture_->from_text( window->renderer(), message, font_, color_);
}



SbMessage::~SbMessage()
{
  font_ = nullptr;
}



SbFpsDisplay::SbFpsDisplay(TTF_Font *font, double x, double y, double width, double height)
  : SbMessage(x,y,width,height)
{
  name_ = "fps";
  set_font(font);
  start_timer();
}



void
SbFpsDisplay::handle_event(const SDL_Event& event)
{
  SbObject::handle_event( event );
  if ( event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f ) {
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_LALT]){
      render_me_ = !render_me_;
    }
  }
}


void
SbFpsDisplay::set_number_frames( unsigned int n )
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

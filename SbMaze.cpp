/* SbMaze, guide a ball through a maze
author: Ulrike Hager
*/

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <iomanip>
#include <iterator>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "SbTexture.h"
#include "SbTimer.h"
#include "SbWindow.h"
#include "SbObject.h"
#include "SbFont.h"

#include "SbMaze.h"


/////  globals /////
SbWindow* SbObject::window;

/*! Ball implementation
 */
Ball::Ball(const SbDimension* ref)
  : SbObject(SDL_Rect{(int)(0.9*ref->w), (int)(0.92*ref->h), 25, 25}, ref)
{
  velocity_y_ = 0;
  velocity_x_ = 0;
  velocity_ = 1.0/5000.0;
  texture_ = std::make_shared<SbTexture>();
  texture_->from_file(window->renderer(), "resources/ball.png", bounding_rect_.w, bounding_rect_.h );
  name_ = "ball";
}


bool
Ball::check_goal(const Goal& goal)
{
  goal_ = false;
  SbHitPosition hit = check_hit(goal);
  if ( hit != SbHitPosition::none ) {
      const SDL_Rect& target = goal.bounding_rect();
    bounding_rect_.x =  target.x + ( target.w - bounding_rect_.w ) / 2;  
    bounding_rect_.y =  target.y + ( target.h - bounding_rect_.h ) / 2;
    move_bounding_box();
    goal_ = true;
  }
  return goal_;
}



void
Ball::handle_event(const SDL_Event& event)
{
  double sensitivity = 1.0 ; // controller needs slower acceleration
  SbControlDir direction = SbControlDir::none;

  if( event.type == SDL_KEYDOWN && event.key.repeat == 0  ) {
    switch( event.key.keysym.sym  ) {
    case SDLK_UP: direction = SbControlDir::up; break;
    case SDLK_DOWN: direction = SbControlDir::down; break;
    case SDLK_LEFT: direction = SbControlDir::left; break;
    case SDLK_RIGHT: direction = SbControlDir::right; break;
    }
  }
  else if( event.type == SDL_CONTROLLERAXISMOTION &&  event.jaxis.which == 0 ) {
    switch ( event.jaxis.axis ) {
    case 0: //X axis motion
      sensitivity = 0.1;
      if ( event.jaxis.value < -CONTROLLER_DEADZONE )
	direction = SbControlDir::left;
      else if ( event.jaxis.value > CONTROLLER_DEADZONE )
	direction = SbControlDir::right;
      break;
    case 1:
      sensitivity = 0.1;
      if ( event.jaxis.value < -CONTROLLER_DEADZONE )
	direction = SbControlDir::up;
      else if ( event.jaxis.value > CONTROLLER_DEADZONE )
	direction = SbControlDir::down;
      break;
    }
  }
  else if( event.type == SDL_JOYAXISMOTION &&  event.jaxis.which == 0 ) {
    switch ( event.jaxis.axis ) {
    case 0: //X axis motion
      sensitivity = 0.1;
      if ( event.jaxis.value < -CONTROLLER_DEADZONE )
	direction = SbControlDir::left;
      else if ( event.jaxis.value > CONTROLLER_DEADZONE )
	direction = SbControlDir::right;
      break;
    case 1:
      sensitivity = 0.1;
      if ( event.jaxis.value < -CONTROLLER_DEADZONE )
	direction = SbControlDir::up;
      else if ( event.jaxis.value > CONTROLLER_DEADZONE )
	direction = SbControlDir::down;
      break;
    }
  }
  else {
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_UP] )
      direction = SbControlDir::up; //velocity_y_ -= velocity_;
    if (state[SDL_SCANCODE_DOWN] )
      direction = SbControlDir::down; //velocity_y_ += velocity_;
    if (state[SDL_SCANCODE_LEFT] )
      direction = SbControlDir::left; //velocity_x_ -= velocity_;
    if (state[SDL_SCANCODE_RIGHT])
      direction = SbControlDir::right; //velocity_x_ += velocity_;
  }


  switch (direction) {
  case SbControlDir::up :
    if (velocity_y_ > -1*velocity_max_) velocity_y_ -= ( velocity_ * sensitivity );
    break;
  case SbControlDir::down :
    if (velocity_y_ < velocity_max_) velocity_y_ +=  ( velocity_ * sensitivity );
    break;
  case SbControlDir::left :
    if (velocity_x_ > -1*velocity_max_) velocity_x_ -=  ( velocity_ * sensitivity );
    break;
  case SbControlDir::right :
    if (velocity_x_ < velocity_max_) velocity_x_ +=  ( velocity_ * sensitivity );
    break;
  default:
    break;
  }
    /*
    */
}



int
Ball::move(const std::vector<std::unique_ptr<SbObject>>& level)
{
  int result = 0;
  if ( goal_ ) {
    return result;
  }
  Uint32 deltaT = timer_.get_time();
  int x_velocity = (int)( window->width() * velocity_x_ * deltaT);
  int y_velocity = (int)( window->height() * velocity_y_ * deltaT);  
  bounding_rect_.y += y_velocity;
  bounding_rect_.x += x_velocity;

  int hits = 0 ;   // can only hit max 2 tiles at once
  for (auto& tile: level){
    SbHitPosition hit = check_hit(*tile);
    if ( hit == SbHitPosition::none )
      continue;
    else {
      ++hits;
      switch (hit) {
      case SbHitPosition::left :
	if (velocity_x_ > 0 )
	  velocity_x_ *= -1*momentum_loss_;  
	break;
      case SbHitPosition::right :
	if (velocity_x_ < 0 )
	  velocity_x_ *= -1*momentum_loss_;  
	break;
      case SbHitPosition::top :
	if (velocity_y_ > 0 )
	  velocity_y_ *= -1*momentum_loss_;  
	break;
      case SbHitPosition::bottom :
	if (velocity_y_ < 0 )
	  velocity_y_ *= -1*momentum_loss_;  
	break;
      default:
	--hits;
	break;
      }
      if ( hits == 2 )
	break;
    }
  }
  move_bounding_box();
  timer_.start();
  return result;
}



void
Ball::reset()
{
  goal_ = false;
  velocity_x_ = 0;
  velocity_y_ = 0;
  bounding_rect_.x = (int)(0.9*reference_->w);
  bounding_rect_.y = (int)(0.92*reference_->h);
  move_bounding_box();
  timer_.start();
}




/*! Tile implementation
 */
Tile::Tile(int x, int y, int width, int height, const SbDimension* ref)
  : SbObject(SDL_Rect{x, y, width, height}, ref)
{
  SDL_Color color = {40, 40, 160, 0};
  texture_ = std::make_shared<SbTexture>();
  texture_->from_rectangle( window->renderer(), bounding_rect_.w, bounding_rect_.h, color );
  name_ = "tile";
}


Tile::Tile( SbRectangle bounding_box, const SbDimension* ref )
  : SbObject( bounding_box, ref)
{
  SDL_Color color = {40, 40, 160, 0};
  texture_ = std::make_shared<SbTexture>();
  texture_->from_rectangle( window->renderer(), bounding_rect_.w, bounding_rect_.h, color );
  name_ = "tile";
}



/*! Goal implementation
 */
Goal::Goal(int x, int y, int width, int height, const SbDimension* ref)
  : SbObject(SDL_Rect{x, y, width, height}, ref)
{
  texture_ = std::make_shared<SbTexture>();
  texture_->from_file(window->renderer(), "resources/goal.png", bounding_rect_.w, bounding_rect_.h );
  name_ = "goal";
}


Goal::Goal(SbRectangle box, const SbDimension* ref)
  : SbObject(box, ref)
{
  texture_ = std::make_shared<SbTexture>();
  texture_->from_file(window->renderer(), "resources/goal.png", bounding_rect_.w, bounding_rect_.h );
  name_ = "goal";
}



/*! Level implementation
 */
Level::Level(int num, std::shared_ptr<TTF_Font> font, const SbDimension* window_ref)
  : level_num_(num)
  , time_message_(SbRectangle{0.9,0,0.1,0.07}, window_ref)
{
  create_level(level_num_);
  time_message_.set_font(font);
}


void
Level::create_level(uint32_t num)
{
  if ( !tiles_.empty() )
    tiles_.clear();
  
  if (num > levels.size() )
    throw std::runtime_error("[Level::create_level] No level found for level number = " + std::to_string(num)  );

  dimension_ = levels.at(num).dimension;
  std::vector< SbRectangle > &coords = (levels.at(num).tiles);
  SbRectangle& goal = levels.at(num).goal;

  for ( auto box: coords ){
    tiles_.emplace_back( std::unique_ptr<SbObject>(new Tile( box, get_dimension() ) ) );
  }
  goal_ = std::unique_ptr<Goal>( new Goal{ goal, get_dimension() } );
}



void
Level::render(const SDL_Rect &camera)
{
    for (auto& t: tiles_)
      t->render(camera);
    goal_->render( camera );
  std::stringstream strstr;
  double time = time_message_.time()/1000.0;
  strstr << std::fixed << std::setprecision(1) << time << " s" ;
  time_message_.set_text( strstr.str() );
  time_message_.render();
}


void
Level::update_size()
{
  time_message_.update_size();
}



Maze::Maze()
{
  SbObject::window = &window_ ;

  for (int i = 0; i < SDL_NumJoysticks(); ++i) {
    if (SDL_IsGameController(i)) {
        game_controller_ = SDL_GameControllerOpen(i);
        if (game_controller_) {
            break;
        }
    }
  }

  levels.emplace_back(dim0, lev0, goal0);
  levels.emplace_back(dim1, lev1, goal1);

  initialize();

}


Maze::~Maze()
{
  SDL_GameControllerClose( game_controller_ );
  game_controller_ = nullptr;
}


void
Maze::initialize()
{
  camera_ = { 0, 0, window_.width(), window_.height() };
  
  SbFont font("resources/FreeSans.ttf", 120 );
  // font_ = std::shared_ptr<TTF_Font>( TTF_OpenFont( "resources/FreeSans.ttf", 120 ), DeleteFont() );
  // if ( !font_ )
  //   throw std::runtime_error( "TTF_OpenFont: " + std::string( TTF_GetError() ) );

  level_ = std::unique_ptr<Level>( new Level(current_level_, font.font(), window_.get_dimension() ) );
  ball_ = std::unique_ptr<Ball>( new Ball(level_->get_dimension()) );
  fps_display_ = std::unique_ptr<SbFpsDisplay>( new SbFpsDisplay( font.font(), SbRectangle{0, 0, 0.06, 0.035}, window_.get_dimension() ) );
  highscore_ = std::unique_ptr<SbHighScore> (new SbHighScore( font.font(), SbRectangle{0.2,0.4,0.6,0.23}, window_.get_dimension() ) );
  highscore_->savefile = "maze.save";
  highscore_->prefix = "Time:" ;
  highscore_->postfix = "s";
  highscore_->set_precision(2);
}


void
Maze::reset()
{
  if ( (++current_level_) == levels.size() ) {
    current_level_ = 0;
  }
  level_->create_level( current_level_ );
  ball_->reset();
  level_->start_timer();
  reset_timer_.reset();
  in_goal_ = false;
}


Uint32
Maze::reset_game(Uint32 interval, void *param )
{
  ((Maze*)param)->reset();
  return(0);
}



void
Maze::run()
{
    SDL_Event event;
    bool quit = false;

    level_->start_timer();
    
    while (!quit) {
      /// begin event polling
      while( SDL_PollEvent( &event ) ) {
	if (event.type == SDL_QUIT) quit = true;
	else if (event.type == SDL_KEYDOWN ) {
	  switch ( event.key.keysym.sym ) {
	  case SDLK_ESCAPE:
	    quit = true;
	    break;
	  }
	}
	else if (   event.type == SDL_CONTROLLERBUTTONDOWN
		    && event.cbutton.which == 0
		    && event.cbutton.button == SDL_CONTROLLER_BUTTON_B ) {
	  quit = true;
	}
	if (window_.handle_event(event) ){
	  ball_->update_size();
	  fps_display_->update_size();
	  level_->update_size();
	}
	ball_->handle_event(event);
	fps_display_->handle_event(event);
      }
      /// end event polling

      	if ( reset_timer_.get_time() > 1500 )
	  reset();
	

      ball_->move(level_->tiles());
      ball_->center_camera(camera_, LEVEL_WIDTH, LEVEL_HEIGHT);
      if ( !in_goal_ ) {
	in_goal_ = ball_->check_goal(level_->goal());
	if (in_goal_) {
	  //	  SDL_AddTimer(2000, Maze::reset_game, this);
	  reset_timer_.start();
	  level_->stop_timer();
	  highscore_->check_highscore( level_->time(), &SbHighScore::lower, current_level_, 0.001 );
	}
      }
      fps_display_->update();
      
      SDL_RenderClear( window_.renderer() );
      level_->render( camera_ );
      fps_display_->render();
      ball_->render( camera_ );
      if ( reset_timer_.get_time() > 0 )
	highscore_->render();
      SDL_RenderPresent( window_.renderer() );

    }
}



int main()
{
  sdl_init();
  try {
    Maze maze;
    maze.run();
  }
  catch (const std::exception& expt) {
    std::cerr << expt.what() << std::endl;
  }
  sdl_quit();
  return 0;
}
  






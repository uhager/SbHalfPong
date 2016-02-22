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

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "SbTexture.h"
#include "SbTimer.h"
#include "SbWindow.h"
#include "SbObject.h"

#include "SbMaze.h"


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int LEVEL_WIDTH = 2000;
const int LEVEL_HEIGHT = 1500;

/////  globals /////
SbWindow* SbObject::window;
TTF_Font *fps_font = nullptr;


void
close()
{
  TTF_CloseFont( fps_font );
  fps_font = nullptr;
  TTF_Quit();
}


/*! Ball implementation
 */
Ball::Ball()
  : SbObject((int)(0.9*LEVEL_WIDTH), (int)(0.92*LEVEL_HEIGHT), 25, 25)
{
  //  bounding_box_ = {};
  velocity_y_ = 0;
  velocity_x_ = 0;
  velocity_ = 1.0/4000.0;
  texture_ = new SbTexture();
  texture_->from_file(window->renderer(), "resources/ball.png", bounding_rect_.w, bounding_rect_.h );
  name_ = "ball";
}


void
Ball::center_camera(SDL_Rect& camera) 
{
  camera.x = pos_x() + width()/2 - window->width()/2;
  camera.y = pos_y() + height()/2 - window->height()/2;
  if ( camera.x < 0 )
      camera.x = 0;
  else if ( camera.x > LEVEL_WIDTH - camera.w )
      camera.x = LEVEL_WIDTH - camera.w;
  
  if ( camera.y < 0 )
      camera.y = 0;
  else if( camera.y > LEVEL_HEIGHT - camera.h )
      camera.y = LEVEL_HEIGHT - camera.h;
}



bool
Ball::check_goal(const Goal& goal)
{
  bool result = false;
  SbHitPosition hit = check_hit(goal);
  if ( hit != SbHitPosition::none ) {
      const SDL_Rect& target = goal.bounding_rect();
    bounding_rect_.x =  target.x + ( target.w - bounding_rect_.w ) / 2;  
    bounding_rect_.y =  target.y + ( target.h - bounding_rect_.h ) / 2;
    move_bounding_box();
    result = true;
  }
  return result;
}



void
Ball::handle_event(const SDL_Event& event)
{
  
  SbObject::handle_event( event );
  if( event.type == SDL_KEYDOWN && event.key.repeat == 0  ) {
    switch( event.key.keysym.sym  ) {
    case SDLK_UP: velocity_y_ -= velocity_; break;
    case SDLK_DOWN: velocity_y_ += velocity_; break;
    case SDLK_LEFT: velocity_x_ -= velocity_; break;
    case SDLK_RIGHT: velocity_x_ += velocity_; break;
    }
  }
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
 
  if (bounding_rect_.x <= 0) {
    if ( velocity_x_ < 0 ) velocity_x_ *= -1*momentum_loss_;  
  }
  else if ( bounding_rect_.x + bounding_rect_.w >= LEVEL_WIDTH ){
    if ( velocity_x_ > 0 ) velocity_x_ *= -1*momentum_loss_;  
  }
  if ( bounding_rect_.y <= 0 ) {
    if ( velocity_y_ < 0 ) velocity_y_ *= -1*momentum_loss_;
  }
  else if ( bounding_rect_.y + bounding_rect_.h >= LEVEL_HEIGHT ) {
    if ( velocity_y_ > 0 ) velocity_y_ *= -1*momentum_loss_;
  }
 
  move_bounding_box();
  timer_.start();
  return result;
}



void
Ball::reset()
{
  goal_ = 0;
  velocity_x_ = 0;
  velocity_y_ = 0;
  bounding_rect_.x = (int)(0.9*LEVEL_WIDTH);
  bounding_rect_.y = (int)(0.92*LEVEL_HEIGHT);
  move_bounding_box();
  timer_.start();
}



Uint32
Ball::resetball(Uint32 interval, void *param )
{
  ((Ball*)param)->reset();
  return(0);
}


Tile::Tile(int x, int y, int width, int height)
  : SbObject(x, y, width, height)
{
  SDL_Color color = {40, 40, 160, 0};
  texture_ = new SbTexture();
  texture_->from_rectangle( window->renderer(), bounding_rect_.w, bounding_rect_.h, color );
  name_ = "tile";
}


Goal::Goal(int x, int y, int width, int height)
  : SbObject(x, y, width, height)
{
  texture_ = new SbTexture();
  texture_->from_file(window->renderer(), "resources/goal.png", bounding_rect_.w, bounding_rect_.h );
  name_ = "goal";
}



Level::Level(int num, TTF_Font* font)
  : width_(LEVEL_WIDTH), height_(LEVEL_HEIGHT), level_num_(num)
  ,  goal_((int)(0.4*LEVEL_WIDTH), (int)(0.48*LEVEL_HEIGHT), (int)(0.03*LEVEL_WIDTH), (int)(0.03*LEVEL_WIDTH) )
  , time_message_(0.9,0,0.1,0.07)
{
  create_level(level_num_);
  time_message_.set_font(font);
}



void
Level::create_level(int num)
{

  std::vector<std::vector<double>> coords;
  if ( num == 1) {
    coords = std::vector<std::vector<double>>{{0,0,1.0,0.05}, {0.95,0.0,0.05,1.0}, {0.0,0.,0.05,1.0}, {0.0, 0.95, 1.0, 0.05}  // outer borders
    , {0.45,0.45,0.1,0.1},{0.35,0.35,0.1,0.1}, {0.55,0.35,0.1,0.1}, {0.55,0.55,0.1,0.1}, {0.35,0.55,0.1,0.1}    // central boxes
	};
  }
  else
    return;
  for (unsigned int i = 0; i < coords.size() ; ++i ){
    int x = coords.at(i).at(0) * width_;
    int y = coords.at(i).at(1) * height_;
    int w = coords.at(i).at(2) * width_;
    int h = coords.at(i).at(3) * height_;
    tiles_.emplace_back( std::unique_ptr<SbObject>(new Tile(x, y, w, h)) );
  }
}


void
Level::render(const SDL_Rect &camera)
{
  for (auto& t: tiles_)
    t->render(camera);
  goal_.render( camera );
  std::stringstream strstr;
  strstr << std::fixed << std::setprecision(1) << ( timer_.get_time()/1000.0 ) << " s" ;
  time_message_.set_text( strstr.str() );
  time_message_.render();
}



Maze::Maze()
{
  window_.initialize("Maze", SCREEN_WIDTH, SCREEN_HEIGHT);
  SbObject::window = &window_ ;
  initialize();

}


Maze::~Maze()
{
  TTF_CloseFont( font_ );
  font_ = nullptr;
  TTF_Quit();
}



void
Maze::initialize()
{
  camera_ = { 0, 0, window_.width(), window_.height() };
  
  font_ = TTF_OpenFont( "resources/FreeSans.ttf", 120 );
  if ( !font_ )
    throw std::runtime_error( "TTF_OpenFont: " + std::string( TTF_GetError() ) );

  ball_ = std::unique_ptr<Ball>( new Ball );
  level_ = std::unique_ptr<Level>( new Level(1, font_) );
  fps_display_ = std::unique_ptr<SbFpsDisplay>( new SbFpsDisplay( font_ ) );
}



void
Maze::reset()
{
  ball_->reset();
  level_->start_timer();
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
      while( SDL_PollEvent( &event ) ) {
	if (event.type == SDL_QUIT) quit = true;
	else if (event.type == SDL_KEYDOWN ) {
	  switch ( event.key.keysym.sym ) {
	  case SDLK_ESCAPE:
	    quit = true;
	    break;
	  }
	}
	window_.handle_event(event);
	ball_->handle_event(event);

      }
      ball_->move(level_->tiles());
      ball_->center_camera(camera_);
      bool is_goal = ball_->check_goal(level_->goal());
      if (is_goal) {
	SDL_AddTimer(2000, Maze::reset_game, this);
	level_->stop_timer();
      }
      fps_display_->update();
      
      SDL_RenderClear( window_.renderer() );
      level_->render( camera_ );
      fps_display_->render();
      ball_->render( camera_ );
      SDL_RenderPresent( window_.renderer() );

    }
}





int main()
{

  try {
    Maze maze;
    maze.run();
  }
  catch (const std::exception& expt) {
    std::cerr << expt.what() << std::endl;
  }
  return 0;
}
  






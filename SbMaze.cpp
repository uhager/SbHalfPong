/* SbMaze, guide a ball through a maze
author: Ulrike Hager
*/

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <memory>
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


SbHitPosition
Ball::check_hit(const std::unique_ptr<Tile>& tile)
{
  SbHitPosition result = SbHitPosition::none;
  const SDL_Rect& hit_box = tile->bounding_rect();
  bool in_xrange = false, in_yrange = false, x_hit_left = false, x_hit_right = false, y_hit_top = false, y_hit_bottom = false ;

  if ( bounding_rect_.x + bounding_rect_.w/2 >= hit_box.x &&
       bounding_rect_.x + bounding_rect_.w/2 <= hit_box.x + hit_box.w )
    in_xrange = true;
      
  if ( bounding_rect_.y + bounding_rect_.h/2 >= hit_box.y  &&
       bounding_rect_.y + bounding_rect_.h/2 <= hit_box.y + hit_box.h)
     in_yrange = true;

  if ( bounding_rect_.x + bounding_rect_.w >= hit_box.x               &&
       bounding_rect_.x                   <= hit_box.x + hit_box.w ) {
    if ( bounding_rect_.x > hit_box.x ) 
      x_hit_right = true;
    else if ( bounding_rect_.x + bounding_rect_.w < hit_box.x + hit_box.w)
      x_hit_left = true;
  }
  if ( bounding_rect_.y + bounding_rect_.h >= hit_box.y               &&
       bounding_rect_.y                   <= hit_box.y + hit_box.h ) {
    if ( bounding_rect_.y > hit_box.y ) 
      y_hit_bottom = true;
    else if ( bounding_rect_.y + bounding_rect_.h < hit_box.y + hit_box.h )
      y_hit_top = true;
  }
  
  if ( x_hit_left && in_yrange ) 
    result = SbHitPosition::left;
  else if (x_hit_right && in_yrange)
    result = SbHitPosition::right;
  else if (y_hit_top && in_xrange)
    result = SbHitPosition::top;
  else if (y_hit_bottom && in_xrange)
    result = SbHitPosition::bottom;
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
Ball::move(const std::vector<std::unique_ptr<Tile>>& level)
{
  int result = 0;
  Uint32 deltaT = timer_.get_time();
  int x_velocity = (int)( window->width() * velocity_x_ * deltaT);
  int y_velocity = (int)( window->height() * velocity_y_ * deltaT);  
  bounding_rect_.y += y_velocity;
  bounding_rect_.x += x_velocity;

  int hits = 0 ;   // can only hit max 2 tiles at once
  for (auto& tile: level){
    SbHitPosition hit = check_hit(tile);
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
  velocity_x_ = 0;
  velocity_y_ = 0;
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



std::vector<std::unique_ptr<Tile>>
create_level()
{
  std::vector<std::unique_ptr<Tile>> result;
  std::vector<std::vector<double>> coords{{0,0,1.0,0.05}, {0.95,0.0,0.05,1.0}, {0.0,0.,0.05,1.0}, {0.0, 0.95, 1.0, 0.05}, {0.45,0.45,0.1,0.1}};
  for (unsigned int i = 0; i < coords.size() ; ++i ){
    int x = coords.at(i).at(0)*LEVEL_WIDTH;
    int y = coords.at(i).at(1)*LEVEL_HEIGHT;
    int w = coords.at(i).at(2)*LEVEL_WIDTH;
    int h = coords.at(i).at(3)*LEVEL_HEIGHT;
    result.emplace_back( std::unique_ptr<Tile>(new Tile(x, y, w, h)) );
  }
  return result;
}



int main()
{

  try {
    SbWindow window;
    window.initialize("Maze", SCREEN_WIDTH, SCREEN_HEIGHT);
    SbObject::window = &window ;
    SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    Ball ball;
    std::vector<std::unique_ptr<Tile>> level = create_level();
    
    fps_font = TTF_OpenFont( "resources/FreeSans.ttf", 120 );
    if ( !fps_font )
      throw std::runtime_error( "TTF_OpenFont: " + std::string( TTF_GetError() ) );

    SbFpsDisplay fps_display( fps_font );

    SDL_Event event;
    bool quit = false;
    
    int frame_counter = 0;

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
	window.handle_event(event);
	ball.handle_event(event);

      }
      ball.move(level);
      ball.center_camera(camera);
      fps_display.update();
      
      SDL_RenderClear( window.renderer() );
      for (auto& t: level)
	t->render(camera);
      fps_display.render();
      ball.render( camera );
      SDL_RenderPresent( window.renderer() );
      
      ++frame_counter;      
    }
  }
  catch (const std::exception& expt) {
    std::cerr << expt.what() << std::endl;
  }
  close();
  return 0;
}
  






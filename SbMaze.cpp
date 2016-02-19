/* SbMaze, guide a ball through a maze
author: Ulrike Hager
*/

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>

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
  : SbObject(50, 300, 25, 25)
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
Ball::move()
{
  int result = 0;
  Uint32 deltaT = timer_.get_time();
  int x_velocity = (int)( window->width() * velocity_x_ * deltaT);
  int y_velocity = (int)( window->height() * velocity_y_ * deltaT);  
  bounding_rect_.y += y_velocity;
  bounding_rect_.x += x_velocity;
  
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



int main()
{

  try {
    SbWindow window;
    window.initialize("Maze", SCREEN_WIDTH, SCREEN_HEIGHT);
    SbObject::window = &window ;
    SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    Ball ball;
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
      ball.move();
      ball.center_camera(camera);
      fps_display.update();
      
      SDL_RenderClear( window.renderer() );
      fps_display.render();
      ball.render(camera.x, camera.y);
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
  






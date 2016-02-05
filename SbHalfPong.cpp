/* SbHalfPong, sort of like Pong but for one
author: Ulrike Hager
*/

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <climits>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "SbTexture.h"
#include "SbTimer.h"
#include "SbWindow.h"
#include "SbObject.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


class Paddle : public SbObject
{
public:
  Paddle();
  void handle_event(const SDL_Event& event);
  int move();
};



class Ball : public SbObject
{
public:
  Ball();
  //  void handle_event(const SDL_Event& event);
  /*! \retval 1 if ball in goal
    \retval 0 else
   */
  int move(const SDL_Rect& paddleBox);
  /*! Reset after goal.
   */
  void reset();
  static Uint32 resetball(Uint32 interval, void *param );

private:
  int goal = 0;
};



TTF_Font *fps_font = nullptr;



/*! Paddle implementation
 */

Paddle::Paddle()
  : SbObject(SCREEN_WIDTH - 70, 200, 20, 140)
{
  //  bounding_rect_ = {}; 
  velocity_y_ = 0;
  velocity_ = 1200;
  SDL_Color color = {210, 160, 10, 0};
  texture_ = new SbTexture();
  texture_->from_rectangle( window->renderer(), bounding_rect_.w, bounding_rect_.h, color );
}



void
Paddle::handle_event(const SDL_Event& event)
{
  SbObject::handle_event( event );
  if( event.type == SDL_KEYDOWN && event.key.repeat == 0 ) {
    switch( event.key.keysym.sym ) {
    case SDLK_UP: velocity_y_ -= velocity_; break;
    case SDLK_DOWN: velocity_y_ += velocity_; break;
    }
  }
  else if( event.type == SDL_KEYUP && event.key.repeat == 0 ) {
    switch( event.key.keysym.sym ) {
    case SDLK_UP: velocity_y_ += velocity_; break;
    case SDLK_DOWN: velocity_y_ -= velocity_; break;
    }
  }
}



int
Paddle::move()
{
  Uint32 deltaT = timer_.getTime();
  int velocity = ( window->height() / velocity_y_ )* deltaT; 
  bounding_rect_.y += velocity;
  if( ( bounding_rect_.y < 0 ) || ( bounding_rect_.y + bounding_rect_.h > window->height() ) ) {
    bounding_rect_.y -= velocity;
  }
  move_bounding_box();
  timer_.start();
  return 0;
}
    



/*! Ball implementation
 */
Ball::Ball()
  : SbObject(50, 300, 25, 25)
{
  //  bounding_box_ = {};
  velocity_y_ = 1500;
  velocity_x_ = 1500;
  velocity_ = 1500;
  texture_ = new SbTexture();
  texture_->from_file(window->renderer(), "resources/ball.png", bounding_rect_.w, bounding_rect_.h );
}




int
Ball::move(const SDL_Rect& paddleBox)
{
  if ( goal ) return 0;
  Uint32 deltaT = timer_.getTime();
  int x_velocity = ( window->width() / velocity_x_ ) * deltaT;
  int y_velocity = ( window->height() / velocity_y_ ) * deltaT;  
  bounding_rect_.y += y_velocity;
  bounding_rect_.x += x_velocity;
  if ( bounding_rect_.x + bounding_rect_.w >= window->width() ) {
    goal = 1;
    bounding_rect_.x = 0;
    bounding_rect_.y = window->height() / 2 ;
    move_bounding_box();
    return goal;
  }
  
  bool in_xrange = false, in_yrange = false, x_hit = false, y_hit = false ;
  if ( bounding_rect_.x + bounding_rect_.w/2 >= paddleBox.x &&
       bounding_rect_.x - bounding_rect_.w/2 <= paddleBox.x + paddleBox.w )
    in_xrange = true;
      
  if ( bounding_rect_.y + bounding_rect_.h/2 >= paddleBox.y  &&
       bounding_rect_.y - bounding_rect_.h/2 <= paddleBox.y + paddleBox.h)
     in_yrange = true;

  if ( bounding_rect_.x + bounding_rect_.w >= paddleBox.x               &&
       bounding_rect_.x                   <= paddleBox.x + paddleBox.w )
    x_hit = true;

  if ( bounding_rect_.y + bounding_rect_.h >= paddleBox.y               &&
       bounding_rect_.y                   <= paddleBox.y + paddleBox.h )
    y_hit = true;

  if ( ( x_hit && in_yrange ) || bounding_rect_.x <= 0  )
    velocity_x_ *= -1;
  if ( ( y_hit && in_xrange ) || bounding_rect_.y <= 0 || ( bounding_rect_.y + bounding_rect_.h >= window->height() ) )
    velocity_y_ *= -1;
 
  move_bounding_box();
  timer_.start();
  return goal;
}


void
Ball::reset()
{
  goal = 0;
  timer_.start();
}



Uint32
Ball::resetball(Uint32 interval, void *param )
{
  // SDL_Event event;
  // SDL_UserEvent userevent;
  
  // userevent.type = SDL_USEREVENT;
  // userevent.code = 0;
  // userevent.data1 = &reset();
  // userevent.data2 = nullptr;

  // event.type = SDL_USEREVENT;
  // event.user = userevent;
  
  // SDL_PushEvent(&event);
  ((Ball*)param)->reset();
  return(0);
}



SbWindow* SbObject::window;

int main()
{
  try {
    SbWindow window;
    window.initialize("Half-Pong", SCREEN_WIDTH, SCREEN_HEIGHT);
    SbObject::window = &window ;
    Paddle paddle;
    Ball ball;
    SbTexture *fps_texture = new SbTexture();
    SDL_Color fps_color = {210, 160, 10, 0};
    SbTimer fps_timer;
    
    fps_font = TTF_OpenFont( "resources/FreeSans.ttf", 18 );
    if ( !fps_font )
      throw std::runtime_error( "TTF_OpenFont: " + std::string( TTF_GetError() ) );

    SDL_TimerID reset_timer;
    SDL_Event event;
    bool quit = false;

    int fps_counter = 0;
    fps_timer.start();
    
    while (!quit) {
      while( SDL_PollEvent( &event ) ) {
	if (event.type == SDL_QUIT) quit = true;
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE ) quit = true;
	window.handle_event(event);
	paddle.handle_event(event);
	ball.handle_event( event );
      }

      if ( fps_counter > 0 && fps_counter < INT_MAX ) {
	double average = double(fps_counter)/ ( fps_timer.getTime()/1000.0 ) ;
	std::string fps_text = std::to_string(int(average)) + " fps";
	fps_texture->from_text( window.renderer(), fps_text, fps_font, fps_color);
      }
      else {
	fps_counter = 0;
	fps_timer.start();
      }
      paddle.move();
      int goal = ball.move( paddle.bounding_rect() );
      if ( goal ) {
	reset_timer = SDL_AddTimer(1000, ball.resetball, &ball);
      }
      SDL_RenderClear( window.renderer() );
      paddle.render();
      ball.render();
      fps_texture->render( window.renderer(), 10,10);
      SDL_RenderPresent( window.renderer() );
      ++fps_counter;  
    }
  }
  catch (const std::exception& expt) {
    std::cerr << expt.what() << std::endl;
  }
  return 0;
}
  

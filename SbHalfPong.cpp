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


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


class Paddle
{
public:
  Paddle();
  ~Paddle();
  void handle_event(const SDL_Event& event);
  void move();
  void render();
  SDL_Rect get_bounding_box() {return bounding_box_;}
  
private:
  SDL_Rect bounding_box_ = {SCREEN_WIDTH - 70, 200, 20, 70}; 
  double y_velocity_ = 0;
  double velocity_ = 0.7;
  SbTexture* texture_ = nullptr;
  SDL_Color color = {210, 160, 10, 0};
  SbTimer timer_;
};



class Ball
{
public:
  Ball();
  ~Ball();
  void move(const SDL_Rect& paddleBox);
  void render();
  
private:
  SDL_Rect bounding_box_ = {50, 300, 25, 25};
  double y_velocity_ = 0.5;
  double x_velocity_ = 0.5;
  double velocity_ = 0.5;
  SbTexture* texture_ = nullptr;
  SbTimer timer_;
  //  SDL_Color color = {210, 160, 10, 0};
};



SDL_Renderer* gRenderer = nullptr;
TTF_Font *fps_font = nullptr;



/*! Paddle implementation
 */

Paddle::Paddle()
{
  texture_ = new SbTexture();
  texture_->from_rectangle( bounding_box_.w, bounding_box_.h, color );
}


Paddle::~Paddle()
{
  delete texture_;
  texture_ = nullptr;
}


void
Paddle::handle_event(const SDL_Event& event)
{
  if( event.type == SDL_KEYDOWN && event.key.repeat == 0 ) {
    switch( event.key.keysym.sym ) {
    case SDLK_UP: y_velocity_ -= velocity_; break;
    case SDLK_DOWN: y_velocity_ += velocity_; break;
    }
  }
  else if( event.type == SDL_KEYUP && event.key.repeat == 0 ) {
    switch( event.key.keysym.sym ) {
    case SDLK_UP: y_velocity_ += velocity_; break;
    case SDLK_DOWN: y_velocity_ -= velocity_; break;
    }
  }
}



void
Paddle::move()
{
  Uint32 deltaT = timer_.getTime();
  int velocity = y_velocity_* deltaT; 
  bounding_box_.y += velocity;
  if( ( bounding_box_.y < 0 ) || ( bounding_box_.y + bounding_box_.h > SCREEN_HEIGHT ) ) {
    bounding_box_.y -= velocity;
  }
  timer_.start();
}
    

void
Paddle::render()
{
  if ( texture_ ) texture_->render( bounding_box_.x, bounding_box_.y);
}



/*! Ball implementation
 */
Ball::Ball()
{
  texture_ = new SbTexture();
  texture_->from_file("resources/ball.png", bounding_box_.w, bounding_box_.h );
}


Ball::~Ball()
{
  delete texture_;
  texture_ = nullptr;
}



void
Ball::move(const SDL_Rect& paddleBox)
{
  Uint32 deltaT = timer_.getTime();
  int y_velocity = y_velocity_ * deltaT;  
  int x_velocity = x_velocity_ * deltaT;
  bounding_box_.y += y_velocity;
  bounding_box_.x += x_velocity;
  if ( bounding_box_.x + bounding_box_.w >= SCREEN_WIDTH ) {
    // goal
    bounding_box_.x = 0;
    bounding_box_.y = SCREEN_HEIGHT / 2 ;
    x_velocity = abs(x_velocity);
    timer_.start();
    return;
  }
  
  bool in_xrange = false, in_yrange = false, x_hit = false, y_hit = false ;
  if ( bounding_box_.x + bounding_box_.w/2 >= paddleBox.x &&
       bounding_box_.x - bounding_box_.w/2 <= paddleBox.x + paddleBox.w )
    in_xrange = true;
      
  if ( bounding_box_.y + bounding_box_.h/2 >= paddleBox.y  &&
       bounding_box_.y - bounding_box_.h/2 <= paddleBox.y + paddleBox.h)
     in_yrange = true;

  if ( bounding_box_.x + bounding_box_.w >= paddleBox.x               &&
       bounding_box_.x                   <= paddleBox.x + paddleBox.w )
    x_hit = true;

  if ( bounding_box_.y + bounding_box_.h >= paddleBox.y               &&
       bounding_box_.y                   <= paddleBox.y + paddleBox.h )
    y_hit = true;

  if ( ( x_hit && in_yrange ) || bounding_box_.x <= 0  )
    x_velocity_ *= -1;
  if ( ( y_hit && in_xrange ) || bounding_box_.y <= 0 || ( bounding_box_.y + bounding_box_.h >= SCREEN_HEIGHT ) )
    y_velocity_ *= -1;
 
  timer_.start();
}



void
Ball::render()
{
  if ( texture_ ) texture_->render( bounding_box_.x, bounding_box_.y);
}



/*! Global functions
 */

int main()
{
  try {
    SbWindow window;
    window.initialize("Half-Pong", SCREEN_WIDTH, SCREEN_HEIGHT);
    gRenderer = window.renderer();
    Paddle paddle;
    Ball ball;
    SbTexture *fps_texture = new SbTexture();
    SDL_Color fps_color = {210, 160, 10, 0};
    SbTimer fps_timer;

    fps_font = TTF_OpenFont( "resources/FreeSans.ttf", 18 );
    if ( !fps_font )
      throw std::runtime_error( "TTF_OpenFont: " + std::string( TTF_GetError() ) );

    SDL_Event event;
    bool quit = false;

    int fps_counter = 0;
    fps_timer.start();
    
    while (!quit) {
      while( SDL_PollEvent( &event ) ) {
	if (event.type == SDL_QUIT) quit = true;
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE ) quit = true;
	paddle.handle_event(event);
      }

      if ( fps_counter > 0 && fps_counter < INT_MAX ) {
	double average = double(fps_counter)/ ( fps_timer.getTime()/1000.0 ) ;
	std::string fps_text = std::to_string(int(average)) + " fps";
	fps_texture->from_text(fps_text, fps_font, fps_color);
      }
      else {
	fps_counter = 0;
	fps_timer.start();
      }
      paddle.move();
      ball.move( paddle.get_bounding_box() );
      SDL_RenderClear( gRenderer );
      paddle.render();
      ball.render();
      fps_texture->render(10,10);
      SDL_RenderPresent( gRenderer );
      ++fps_counter;  
    }
  }
  catch (const std::exception& expt) {
    std::cerr << expt.what() << std::endl;
  }
  return 0;
}
  

/* SbHalfPong, sort of like Pong but for one
author: Ulrike Hager
*/

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <functional>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "SbTexture.h"
#include "SbTimer.h"
#include "SbWindow.h"
#include "SbObject.h"
#include "SbFont.h"

#include "SbHalfPong.h"


SbWindow* SbObject::window;


/*! Paddle implementation
 */
Paddle::Paddle()
  : SbObject(SCREEN_WIDTH - 70, 200, 20, 80)
{
  //  bounding_rect_ = {}; 
  velocity_y_ = 0;
  velocity_ = 1.0/1200.0;
  SDL_Color color = {210, 160, 10, 0};
  texture_ = std::make_shared<SbTexture>();
  texture_->from_rectangle( window->renderer(), bounding_rect_.w, bounding_rect_.h, color );
  name_ = "paddle";
}



void
Paddle::handle_event(const SDL_Event& event)
{
  
  //  SbObject::handle_event( event );
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
  else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
      int mouse_x = -1, mouse_y = -1;
      SDL_GetMouseState( &mouse_x, &mouse_y );
      is_inside( mouse_x, mouse_y );
#ifdef DEBUG
      std::cout << "[Paddle::handle_event] mouse click is " << ( has_mouse()? "inside" : "outside" ) << std::endl;
#endif // DEBUG
    }
  else if (event.type == SDL_MOUSEBUTTONUP) {
    // SDL_GetMouseState( &mouse_x, &mouse_y );
    has_mouse_ = false;
  }
  else if (event.type == SDL_MOUSEMOTION) {
    if ( has_mouse() ) {
      int y_rel = event.motion.yrel;
      bounding_rect_.y += y_rel;
    }
  }
}



int
Paddle::move()
{
  Uint32 deltaT = timer_.get_time();
  int velocity = (int)( window->height() * velocity_y_ * deltaT); 
  bounding_rect_.y += velocity;
  if( ( bounding_rect_.y < 0 ) || ( bounding_rect_.y + bounding_rect_.h > window->height() ) ) {
    bounding_rect_.y -= velocity;
  }
  move_bounding_box();
  timer_.start();
  return 0;
}
    


Spark::Spark(double x, double y, double width, double height)
  : SbObject(x,y,width,height)
{
}



Uint32 
Spark::expire(Uint32 interval, void* param)
{
#ifdef DEBUG
    std::cout << "[Spark::expire] interval " << interval << std::flush;
#endif // DEBUG
  Spark* spark = ((Spark*)param);
#ifdef DEBUG
    std::cout << "[Spark::expire] index " << spark->index_ << std::endl;
#endif // DEBUG
  if (spark) spark->is_dead_ = true;
  return(0);
}





/*! Ball implementation
 */
Ball::Ball()
  : SbObject(50, 300, 25, 25)
{
  velocity_y_ = 1.0/1500.0;
  velocity_x_ = 1.0/1500.0;
  velocity_ = 1.0/1500.0;
  texture_ = std::make_shared<SbTexture>();
  texture_->from_file(window->renderer(), "resources/ball.png", bounding_rect_.w, bounding_rect_.h );
  name_ = "ball";
}



void
Ball::center_in_front(const SDL_Rect& paddleBox)
{
    bounding_rect_.x = paddleBox.x - bounding_rect_.w - 2;
    bounding_rect_.y = paddleBox.y + paddleBox.h / 2 - bounding_rect_.h/2 ;
    move_bounding_box();
}



void
Ball::create_sparks()
{
#ifdef DEBUG
    std::cout << "[Ball::create_sparks]" << std::endl;
#endif // DEBUG
  if (! sparks_.empty() ) {
    sparks_.clear();
  }
  int n_sparks = distr_number(generator_);
  for ( int i = 0 ; i < n_sparks ; ++i ) {
    double x = distr_position(generator_);
    double y = distr_position(generator_);
    double d = distr_size(generator_);
    x += ( bounding_box_.x + bounding_box_.w/2);
    y += ( bounding_box_.y + bounding_box_.h/2);
    Spark toAdd(x, y, d, d);
    toAdd.index_ = i;
    toAdd.set_texture( texture_ );
    toAdd.lifetime_ = Uint32(distr_lifetime(generator_));
    toAdd.timer_.start();
    sparks_.push_back(toAdd);
#ifdef DEBUG
    std::cout << "[Ball::create_sparks] index " << i << " - lifetime " << (sparks_.back()).lifetime() << std::endl;
#endif // DEBUG
  }
}



void
Ball::delete_spark(int index)
{
  std::remove_if( sparks_.begin(), sparks_.end(),
		  [index](Spark& spark) -> bool { return spark.index() == index;} );
}



int
Ball::move(const SDL_Rect& paddleBox)
{
  int result = 0;
  if ( goal_ ) {
    center_in_front(paddleBox);
    return result;
  }
  Uint32 deltaT = timer_.get_time();
  int x_velocity = (int)( window->width() * velocity_x_ * deltaT );
  int y_velocity = (int)( window->height() * velocity_y_ * deltaT );  
  bounding_rect_.y += y_velocity;
  bounding_rect_.x += x_velocity;
  if ( bounding_rect_.x + bounding_rect_.w >= window->width() ) {
    goal_ = 1;
    center_in_front(paddleBox);
    return goal_;
  }
  
  bool in_xrange = false, in_yrange = false, x_hit = false, y_hit_top = false, y_hit_bottom = false ;
  if ( bounding_rect_.x + bounding_rect_.w/2 >= paddleBox.x &&
       bounding_rect_.x + bounding_rect_.w/2 <= paddleBox.x + paddleBox.w )
    in_xrange = true;
      
  if ( bounding_rect_.y + bounding_rect_.h/2 >= paddleBox.y  &&
       bounding_rect_.y + bounding_rect_.h/2 <= paddleBox.y + paddleBox.h)
     in_yrange = true;

  if ( bounding_rect_.x + bounding_rect_.w >= paddleBox.x               &&
       bounding_rect_.x                   <= paddleBox.x + paddleBox.w )
    x_hit = true;

  if ( bounding_rect_.y + bounding_rect_.h >= paddleBox.y               &&
       bounding_rect_.y                   <= paddleBox.y + paddleBox.h ) {
    if ( bounding_rect_.y > paddleBox.y ) 
      y_hit_bottom = true;
    else if ( bounding_rect_.y + bounding_rect_.h < paddleBox.y + paddleBox.h )
      y_hit_top = true;
  }
  
  if ( ( x_hit && in_yrange )   ) {
    if ( velocity_x_ > 0 ) velocity_x_ *= -1;
    create_sparks();
    result = 2;
  }
  else if (bounding_rect_.x <= 0) {
    if ( velocity_x_ < 0 ) velocity_x_ *= -1;  
  }
  
  if ( ( y_hit_bottom && in_xrange )  || bounding_rect_.y <= 0 ) {
    if ( velocity_y_ < 0 ) velocity_y_ *= -1;
  }
  else if ( ( y_hit_top && in_xrange )|| ( bounding_rect_.y + bounding_rect_.h >= window->height() ) ) {
    if ( velocity_y_ > 0 ) velocity_y_ *= -1;
  }
 
  move_bounding_box();
  timer_.start();
  return result;
}



Uint32
Ball::remove_spark(Uint32 interval, void *param, int index )
{
  ((Ball*)param)->delete_spark(index);
  return(0);
}



void
Ball::render()
{
  SbObject::render();
  if ( sparks_.empty() ) 
    return;
  // std::for_each( sparks_.begin(), sparks_.end(),
  // 		   [](Spark& spark) -> void { if ( !spark.is_dead() ) spark.render(); } ); 
  auto iter = sparks_.begin();
  while ( iter != sparks_.end() ) {
    if ( iter->time() > iter->lifetime() ){
      iter = sparks_.erase(iter);
    }
    else {
      iter->render();
      ++iter;
    }
  }
}


void
Ball::reset()
{
  goal_ = 0;
  if ( velocity_x_ > 0 ) velocity_x_ *= -1;
  timer_.start();
}



Uint32
Ball::resetball(Uint32 interval, void *param )
{
  ((Ball*)param)->reset();
  return(0);
}


/*! GameOver implementation
 */
GameOver::GameOver(std::shared_ptr<TTF_Font> font)
  : SbMessage(0.35,0.58,0.3,0.2)
{
  name_ = "gameover" ;
  font_ = font;
  set_text("Game Over");
}



HalfPong::HalfPong()
{
  SbObject::window = &window_ ;
  SbFont font("resources/FreeSans.ttf", 120 );
  // font_ = std::shared_ptr<TTF_Font>( TTF_OpenFont( "resources/FreeSans.ttf", 120 ), DeleteFont() );
  // if ( !font_.get() )
  //     throw std::runtime_error( "TTF_OpenFont: " + std::string( TTF_GetError() ) );

  ball_ = std::unique_ptr<Ball>( new Ball );
  paddle_ = std::unique_ptr<Paddle>( new Paddle );
  fps_display_ = std::unique_ptr<SbFpsDisplay>( new SbFpsDisplay(font.font()) );
  game_over_ = std::unique_ptr<GameOver>( new GameOver( font.font() ) );
  high_score_ = std::unique_ptr<SbHighScore>( new SbHighScore( font.font(), "halfpong.save", "Score:" ) );
  high_score_->set_precision(0);
  lives_ = std::unique_ptr<SbMessage>( new SbMessage(0.2, 0.003, 0.13, 0.07 ) );
  score_text_ = std::unique_ptr<SbMessage>( new SbMessage( 0.5, 0.003, 0.13, 0.07 ) );
  lives_->set_font(font.font());
  score_text_->set_font(font.font());
  lives_->set_text( "Lives: " + std::to_string(goal_counter_) );
  score_text_->set_text( "Score: " + std::to_string(score_) );
  
}


void
HalfPong::run()
{
    std::vector<SbObject*> objects;
    objects.push_back(paddle_.get() );
    objects.push_back(ball_.get() );
    objects.push_back(lives_.get() );
    objects.push_back(score_text_.get() );
    objects.push_back(game_over_.get() );
    objects.push_back(high_score_.get() );
    objects.push_back(fps_display_.get() );

    SDL_Event event;
    bool quit = false;

    while (!quit) {
      while( SDL_PollEvent( &event ) ) {
	if (event.type == SDL_QUIT) quit = true;
	else if (event.type == SDL_KEYDOWN ) {
	  switch ( event.key.keysym.sym ) {
	  case SDLK_ESCAPE:
	    quit = true;
	    break;
	  case SDLK_n: case SDLK_SPACE: case SDLK_RETURN:
	    goal_counter_ = 3;
	    ball_->reset();
	    lives_->set_text( "Lives: " + std::to_string(goal_counter_) );
	    score_ = 0;
	    score_text_->set_text( "Score: " + std::to_string(score_) );
	    break;
	  }
	}
	if ( window_.handle_event( event ) ) {
	  std::for_each( objects.begin(), objects.end(),
			 [] (SbObject* obj) {obj->update_size();} );
	}
	std::for_each( objects.begin(), objects.end(),
		       [event] (SbObject* obj) {obj->handle_event( event );} );

      }
            
      move_objects();
      render( objects );
    }
}


void
HalfPong::move_objects()
{
  if ( goal_counter_ > 0 ) {
    paddle_->move();
    int goal = ball_->move( paddle_->bounding_rect() );
    switch (goal) {
    case 1: 
      --goal_counter_;
      if (goal_counter_ > 0 ) {
	SDL_AddTimer(1000, Ball::resetball, ball_.get());
      }
      else {
	high_score_->check_highscore( score_, &SbHighScore::higher);
      }
      lives_->set_text( "Lives: " + std::to_string(goal_counter_) );
      break;
    case 2:
      ++score_;
      score_text_->set_text( "Score: " + std::to_string(score_) );
      break;
    }
  }
  else
    ball_->move( paddle_->bounding_rect() );
}



void
HalfPong::render(std::vector<SbObject*> objects)
{
  fps_display_->update();

  // render
  SDL_RenderClear( window_.renderer() );
  
  std::for_each( objects.begin(), objects.end(),
		 [](SbObject* obj) {if (obj->name() != "gameover") obj->render(); } );

 
  if ( goal_counter_ == 0 ) {
    game_over_->render();
    high_score_->render();
  }
  SDL_RenderPresent( window_.renderer() );

}



int main()
{
  sdl_init();
  try {
    HalfPong halfpong;
    halfpong.run();
  }
  catch (const std::exception& expt) {
    std::cerr << expt.what() << std::endl;
  }
  sdl_quit();
  return 0;
}
  

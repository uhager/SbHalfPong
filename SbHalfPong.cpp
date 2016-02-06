/* SbHalfPong, sort of like Pong but for one
author: Ulrike Hager
*/

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <climits>
#include <cmath>
#include <random>
#include <algorithm>
#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "SbTexture.h"
#include "SbTimer.h"
#include "SbWindow.h"
#include "SbObject.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


class Ball;
class Spark;
class Paddle;

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
  void render();
  /*! Reset after goal.
   */
  void reset();
  static Uint32 resetball(Uint32 interval, void *param );
  Uint32 remove_spark(Uint32 interval, void *param, int index );

  
private:
  int goal_ = 0;
  std::vector<Spark> sparks_;
  std::default_random_engine generator_;
  std::uniform_int_distribution<int> distr_number { 15, 30 };
  std::normal_distribution<double> distr_position { 0.0, 0.01 };
  std::normal_distribution<double> distr_size { 0.003, 0.002 };
  std::normal_distribution<float> distr_lifetime { 200, 150 };

  void create_sparks();
  void delete_spark(int index);
};



class Spark : public SbObject
{
  friend class Ball;
public:
  Spark(double x, double y, double width, double height);
  ~Spark();

  static Uint32 expire(Uint32 interval, void* param);

  void set_texture(SbTexture* tex) {texture_ = tex;}
  int index() { return index_;}
  bool is_dead() {return is_dead_;}  

private:
  SDL_TimerID spark_timer_;
  int index_ = 0;
  bool is_dead_ = false;
};

  


/*! Paddle implementation
 */

Paddle::Paddle()
  : SbObject(SCREEN_WIDTH - 70, 200, 20, 80)
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
    


Spark::Spark(double x, double y, double width, double height)
  : SbObject(x,y,width,height)
{
}



Spark::~Spark()
{
  texture_ = nullptr;
  SDL_RemoveTimer(spark_timer_);
}



Uint32 
Spark::expire(Uint32 interval, void* param)
{
#ifdef DEBUG
    std::cout << "[Spark::expire] interval " << interval << std::endl;
#endif // DEBUG
  Spark* spark = ((Spark*)param);
#ifdef DEBUG
    std::cout << "[Spark::expire] index " << spark->index_ << std::endl;
#endif // DEBUG
  if (spark) spark->is_dead_ = true;
  return(0);
}



Uint32
Ball::remove_spark(Uint32 interval, void *param, int index )
{
  ((Ball*)param)->delete_spark(index);
  return(0);
}




void
Ball::delete_spark(int index)
{
  std::remove_if( sparks_.begin(), sparks_.end(),
		  [index](Spark& spark) -> bool { return spark.index() == index;} );
  // ((Spark*)param)->set_texture( nullptr );
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



void
Ball::create_sparks()
{
  if (! sparks_.empty() ) sparks_.clear();
  int n_sparks = distr_number(generator_);
  for ( int i = 0 ; i < n_sparks ; ++i ) {
    double x = distr_position(generator_);
    double y = distr_position(generator_);
    double d = distr_size(generator_);
    x += ( bounding_box_[0] + bounding_box_[2]/2);
    y += ( bounding_box_[1] + bounding_box_[3]/2);
    Spark toAdd(x, y, d, d);
    toAdd.set_texture( texture_ );
    toAdd.index_ = i;
    Uint32 lifetime = Uint32(distr_lifetime(generator_));
    if ( lifetime < 0 ) lifetime *= -1;
#ifdef DEBUG
    std::cout << "[Ball::create_sparks] index " << i << " - lifetime " << lifetime << std::endl;
#endif // DEBUG
    //    std::function<Uint32(Uint32,void*)> funct = std::bind(&Ball::remove_spark, this, std::placeholders::_1, std::placeholders::_2, toAdd.index_ );
    sparks_.push_back(toAdd);
    sparks_.back().spark_timer_ = SDL_AddTimer(lifetime, Spark::expire, &sparks_.back());
  }
}



int
Ball::move(const SDL_Rect& paddleBox)
{
  if ( goal_ ) return 0;
  Uint32 deltaT = timer_.getTime();
  int x_velocity = ( window->width() / velocity_x_ ) * deltaT;
  int y_velocity = ( window->height() / velocity_y_ ) * deltaT;  
  bounding_rect_.y += y_velocity;
  bounding_rect_.x += x_velocity;
  if ( bounding_rect_.x + bounding_rect_.w >= window->width() ) {
    goal_ = 1;
    bounding_rect_.x = 0;
    bounding_rect_.y = window->height() / 2 ;
    move_bounding_box();
    return goal_;
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

  if ( ( x_hit && in_yrange ) || bounding_rect_.x <= 0  ) {
    velocity_x_ *= -1;
    if ( x_hit && in_yrange )
      create_sparks();
  }
  if ( ( y_hit && in_xrange ) || bounding_rect_.y <= 0 || ( bounding_rect_.y + bounding_rect_.h >= window->height() ) )
    velocity_y_ *= -1;
 
  move_bounding_box();
  timer_.start();
  return goal_;
}



void
Ball::render()
{
  SbObject::render();
  if ( !sparks_.empty() )
    std::for_each( sparks_.begin(), sparks_.end(),
		   [](Spark& spark) -> void { if ( !spark.is_dead() ) spark.render(); } ); 
}


void
Ball::reset()
{
  goal_ = 0;
  timer_.start();
}



Uint32
Ball::resetball(Uint32 interval, void *param )
{
  ((Ball*)param)->reset();
  return(0);
}



SbWindow* SbObject::window;
TTF_Font *fps_font = nullptr;

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
	reset_timer = SDL_AddTimer(1000, Ball::resetball, &ball);
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
  

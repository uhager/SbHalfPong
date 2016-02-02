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

#include <SbTexture.h>
#include <SbTimer.h>


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 560;


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
  void move(SDL_Rect paddleBox);
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
  

void init();
void close();

SDL_Window* gWindow = nullptr;
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
Ball::move(SDL_Rect paddleBox)
{
  Uint32 deltaT = timer_.getTime();
  int y_velocity = y_velocity_ * deltaT;  
  int x_velocity = x_velocity_ * deltaT;
  bounding_box_.y += y_velocity;
  bounding_box_.x += x_velocity;
  if ( bounding_box_.x + bounding_box_.w >= SCREEN_WIDTH ) {
    bounding_box_.x = 0;
    bounding_box_.y = SCREEN_HEIGHT / 2 ;
    x_velocity = abs(x_velocity);
    timer_.start();
    return;
  }
  if ( bounding_box_.x + bounding_box_.w >= paddleBox.x       &&
       ( bounding_box_.y + bounding_box_.h/2) >= paddleBox.y  &&
       ( bounding_box_.y - bounding_box_.h/2) <= paddleBox.y + paddleBox.h) {
    x_velocity_ *= -1;
  }
  else if ( ( bounding_box_.x <= 0 ) ) {
    x_velocity_ *= -1;
  }
  if ( ( bounding_box_.y <= 0 ) || ( bounding_box_.y + bounding_box_.h >= SCREEN_HEIGHT ) ) {
    y_velocity_ *= -1;
  }
  timer_.start();
}



void
Ball::render()
{
  if ( texture_ ) texture_->render( bounding_box_.x, bounding_box_.y);
}



/*! Global functions
 */
void
init()
{
 if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    std::cerr << "SDL could not initialize! SDL_Error: " <<  SDL_GetError() << std::endl;
    exit(1);
  }
  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
    std::cout << "IMG_Init " << SDL_GetError();
    SDL_Quit();
    exit(1);
  }
  gWindow = SDL_CreateWindow( "Basic half-Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
  if( gWindow == nullptr ){
    std::cerr << "Could not create window. SDL_Error: " <<  SDL_GetError()  << std::endl;
    exit(1);
  }
  gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
  if( gRenderer == nullptr ){
    std::cerr << "Could not create renderer. SDL_Error: " <<  SDL_GetError() << std::endl;
    exit(1);
  }
  SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

  if (TTF_Init() != 0){
    std::cerr << "[SlTextureManager::SlTextureManager] Error in TTF_init: " << SDL_GetError() << std::endl;
    SDL_Quit();
    exit(1);
  }
 
  fps_font = TTF_OpenFont( "resources/FreeSans.ttf", 18 );
  if ( !fps_font )
    throw std::runtime_error( "TTF_OpenFont: " + std::string( TTF_GetError() ) );
}


void close()
{
  SDL_DestroyRenderer( gRenderer );
  SDL_DestroyWindow( gWindow );
  gWindow = nullptr;
  gRenderer = nullptr;

  IMG_Quit();
  SDL_Quit();
}


int main()
{
  try {
    init();
    Paddle paddle;
    Ball ball;
    SbTexture *fps_texture = new SbTexture();
    SDL_Color fps_color = {210, 160, 10, 0};
    SbTimer fps_timer;
    
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
    close();
  }
  catch (const std::exception& expt) {
    std::cerr << expt.what() << std::endl;
  }
  return 0;
}
  

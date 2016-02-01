/* SbHalfPong, sort of like Pong but for one
author: Ulrike Hager
*/

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <climits>

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
  void handleEvent(const SDL_Event& event);
  void move(Uint32 deltaT);
  void render();
  
private:
  int width_ = 20;
  int height_ = 60;
  int xPos_ = SCREEN_WIDTH - 50;
  int yPos_ = 200;
  double yVel_ = 0;
  double velocity_ = 0.6;
  SbTexture* texture_ = nullptr;
  SDL_Color color = {210, 160, 10, 0};
};



class Ball
{
public:
  Ball();
  ~Ball();
  void move(Uint32 deltaT);
  void render();
  
private:
  int width_ = 25;
  int height_ = 25;
  int xPos_ = 50;
  int yPos_ = 300;
  double yVel_ = 0.5;
  double xVel_ = 0.5;
  double velocity_ = 0.5;
  SbTexture* texture_ = nullptr;
  //  SDL_Color color = {210, 160, 10, 0};
};
  

void init();
void close();

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
TTF_Font *fpsFont = nullptr;



/*! Paddle implementation
 */

Paddle::Paddle()
{
  texture_ = new SbTexture();
  texture_->createFromRectangle( width_, height_, color );
}

Paddle::~Paddle()
{
  delete texture_;
  texture_ = nullptr;
}



void
Paddle::handleEvent(const SDL_Event& event)
{
  if( event.type == SDL_KEYDOWN && event.key.repeat == 0 ) {
    switch( event.key.keysym.sym ) {
    case SDLK_UP: yVel_ -= velocity_; break;
    case SDLK_DOWN: yVel_ += velocity_; break;
    }
  }
  else if( event.type == SDL_KEYUP && event.key.repeat == 0 ) {
    switch( event.key.keysym.sym ) {
    case SDLK_UP: yVel_ += velocity_; break;
    case SDLK_DOWN: yVel_ -= velocity_; break;
    }
  }
}


void
Paddle::move(Uint32 deltaT)
{
  int velocity = yVel_* deltaT; 
  yPos_ += velocity;
  if( ( yPos_ < 0 ) || ( yPos_ + height_ > SCREEN_HEIGHT ) ) {
    yPos_ -= velocity;
  }
}
    

void
Paddle::render()
{
  if ( texture_ ) texture_->render( xPos_, yPos_);
}


/*! Ball implementation
 */
Ball::Ball()
{
  texture_ = new SbTexture();
  texture_->createFromFile("resources/ball.png", width_, height_ );
}


Ball::~Ball()
{
  delete texture_;
  texture_ = nullptr;
}



void
Ball::move(Uint32 deltaT)
{
  int y_velocity = yVel_ * deltaT;  
  int x_velocity = xVel_ * deltaT;
  yPos_ += y_velocity;
  xPos_ += x_velocity;
  if( ( yPos_ <= 0 ) || ( yPos_ + height_ >= SCREEN_HEIGHT ) ) {
    yVel_ *= -1;
  }
  if( ( xPos_ <= 0 ) || ( xPos_ + width_ >= SCREEN_WIDTH ) ) {
    xVel_ *= -1;
  }
}



void
Ball::render()
{
  if ( texture_ ) texture_->render( xPos_, yPos_);
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
 
  fpsFont = TTF_OpenFont( "resources/FreeSans.ttf", 18 );
  if ( !fpsFont )
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
    SbTexture *fpsTexture = new SbTexture();
    SDL_Color fpsColor = {210, 160, 10, 0};
    SbTimer fpsTimer, frameTimer;
    
    SDL_Event event;
    bool quit = false;

    int frameCounter = 0;
    fpsTimer.start();
    frameTimer.start();
    
    while (!quit) {
      while( SDL_PollEvent( &event ) ) {
	if (event.type == SDL_QUIT) quit = true;
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE ) quit = true;
	paddle.handleEvent(event);
      }

      if ( frameCounter > 0 && frameCounter < INT_MAX ) {
	double average = double(frameCounter)/ ( fpsTimer.getTime()/1000.0 ) ;
	std::string fpsText = std::to_string(int(average)) + " fps";
	fpsTexture->createFromText(fpsText, fpsFont, fpsColor);
      }
      else {
	frameCounter = 0;
	fpsTimer.start();
      }
      paddle.move( frameTimer.getTime() );
      ball.move( frameTimer.getTime() );
      frameTimer.start();
      SDL_RenderClear( gRenderer );
      paddle.render();
      ball.render();
      fpsTexture->render(10,10);
      SDL_RenderPresent( gRenderer );
      ++frameCounter;  
    }
    close();
  }
  catch (const std::exception& expt) {
    std::cerr << expt.what() << std::endl;
  }
  return 0;
}
  

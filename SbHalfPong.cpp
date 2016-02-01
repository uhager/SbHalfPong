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


const int SCREEN_WIDTH = 680;
const int SCREEN_HEIGHT = 560;


//Texture wrapper class
class SbTexture
{
public:
  SbTexture() = default;
  ~SbTexture();

  SbTexture* createFromFile( const std::string& filename, int width = 0, int height = 0);
  SbTexture* createFromRectangle( int width, int height, const SDL_Color& color );
  SbTexture* createFromText( const std::string& text, const SDL_Color& color );
  void clear();
  void render( int x, int y, SDL_Rect* clip = nullptr);
  int getWidth(){ return width_; }
  int getHeight(){ return height_;}

private:
  SDL_Texture* texture_ = nullptr;
  int width_ = 0;
  int height_ = 0 ;
};

class SbTimer
{
public:
  SbTimer() = default;
  void start();
  void stop();
  /*! All times in ms
   */
  Uint32 getTime();
  bool started(){ return started_ ;}

private:
  Uint32 startTime_ = 0;
  bool started_ = false;
};



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
  double velocity_ = 0.5;
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
  double yVel_ = 0;
  double xVel_ = 0;
  double velocity_ = 0.5;
  SbTexture* texture_ = nullptr;
  //  SDL_Color color = {210, 160, 10, 0};
};
  

void init();
void close();

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
TTF_Font *fpsFont = nullptr;

/*! SbTexture implementation
 */
SbTexture::~SbTexture()
{
  clear();
}


void
SbTexture::clear()
{
  if( texture_ != NULL )
    {
      SDL_DestroyTexture( texture_ );
      texture_ = nullptr;
      width_ = 0;
      height_ = 0;
    }
}



SbTexture*
SbTexture::createFromFile( const std::string& filename, int width, int height )
{
  texture_ = IMG_LoadTexture(gRenderer, filename.c_str());
  
  if( texture_ == nullptr )
    throw std::runtime_error("Unable to create texture from " + filename + " " + SDL_GetError() );

  /*! Should be plotted with the specified width, height 
   */
  if ( width > 0 && height > 0 ){
    width_ = width;
    height_ = height;
  }
  else {
    SDL_QueryTexture( texture_, nullptr, nullptr, &width_, &height_ );
    if ( width > 0 ) {
      width_ = width;
    }
    else if ( height > 0 ) {
      height_ = height;
    }
  }
  
  return this;
}


SbTexture*
SbTexture::createFromRectangle( int width, int height, const SDL_Color& color )
{
  clear();
  texture_ = SDL_CreateTexture(gRenderer, 0, SDL_TEXTUREACCESS_TARGET, width, height);
  if (texture_ == nullptr) 
    throw std::runtime_error("Failed to create texture " + std::string( SDL_GetError() ));
  
  SDL_RenderClear(gRenderer);
  SDL_SetRenderTarget(gRenderer, texture_);
  SDL_SetRenderDrawColor( gRenderer, color.r, color.g, color.b, color.a );
  SDL_Rect sourceRect = {0,0,width,height};
  int check = SDL_RenderFillRect( gRenderer, &sourceRect );
  if ( check != 0 )
    throw std::runtime_error("Couldn't render rectangle: " + std::string( SDL_GetError() ));
  SDL_SetRenderTarget(gRenderer, nullptr);
  SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
  SDL_RenderClear(gRenderer);

  width_ = width;
  height_ = height;
  return this;
}



SbTexture*
SbTexture::createFromText( const std::string& text, const SDL_Color& color )
{
#ifdef DEBUG
  std::cout << "[SbTexture::createFromText]" << std::endl;
#endif
  
  clear();
  SDL_Surface *surf = TTF_RenderText_Solid(fpsFont, text.c_str(), color);
  if (surf == nullptr)
    throw std::runtime_error("Failed to create surface from text: " + std::string( SDL_GetError() ));

  texture_ = SDL_CreateTextureFromSurface(gRenderer, surf);
  SDL_FreeSurface(surf);
  if (texture_ == nullptr) 
    throw std::runtime_error("Failed to create texture " + std::string( SDL_GetError() ));

  SDL_QueryTexture( texture_, nullptr, nullptr, &width_, &height_ );
  return this;
}


void
SbTexture::render( int x, int y, SDL_Rect* sourceRect)
{
  SDL_Rect destRect = { x, y, width_, height_ };
  SDL_RenderCopy( gRenderer, texture_, sourceRect, &destRect );
}



/*! SbTimer implementation
 */

void
SbTimer::start()
{
  started_ = true;
  startTime_ = SDL_GetTicks();
}


void
SbTimer::stop()
{
  started_ =false;
  // When stopped, the timer will return the time interval between start and stop. This is saved in startTime_ until the timer is restarted.
  startTime_ = SDL_GetTicks() - startTime_;
}


Uint32
SbTimer::getTime()
{
  Uint32 time = 0;
  if ( started_ )
    time = SDL_GetTicks() - startTime_;
  else
    time = startTime_;
  return time;
}





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
	fpsTexture->createFromText(fpsText, fpsColor);
      }
      else {
	frameCounter = 0;
	fpsTimer.start();
      }
      paddle.move( frameTimer.getTime() );
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
  

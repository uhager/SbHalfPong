/*! \file SbWindow.h
  part of SDL2-basic
  author: Ulrike Hager
 */

#include <iostream>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "SbWindow.h"


SbWindow::~SbWindow()
{
  close();
}


void
SbWindow::initialize(std::string title, int width, int height)
{
  window_ = SDL_CreateWindow( "Basic half-Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if( window_ == nullptr ){
    std::cerr << "Could not create window. SDL_Error: " <<  SDL_GetError()  << std::endl;
    exit(1);
  }
  renderer_ = SDL_CreateRenderer( window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
  if( renderer_ == nullptr ){
    std::cerr << "Could not create renderer. SDL_Error: " <<  SDL_GetError() << std::endl;
    exit(1);
  }
  SDL_SetRenderDrawColor( renderer_, 0x0, 0x0, 0x0, 0x0 );

  width_ = width;
  height_ = height;
}


void
SbWindow::close()
{
  SDL_DestroyRenderer( renderer_ );
  SDL_DestroyWindow( window_ );
  window_ = nullptr;
  renderer_ = nullptr;

}



void
SbWindow::handle_event(const SDL_Event& event)
{
  new_size_ = false;
  if( event.type == SDL_WINDOWEVENT ) {
    switch( event.window.event ) {
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      width_ = event.window.data1;
      height_ = event.window.data2;
      new_size_ = true;
      break;
    }
  }
  else if( event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_f ) {
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_LALT]) return;  // toggles fps display

    if ( is_fullscreen ) {
      SDL_SetWindowFullscreen( window_, SDL_FALSE );
      SDL_GetWindowSize( window_, &width_, &height_ );
      is_fullscreen = false;
    }
    else {
      SDL_SetWindowFullscreen( window_, SDL_WINDOW_FULLSCREEN_DESKTOP );
      SDL_GetWindowSize( window_, &width_, &height_ );
      is_fullscreen = true;
    }
    new_size_ = true;
  }
}



void sdl_init()
{
 if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER ) < 0 ) {
    std::cerr << "SDL could not initialize! SDL_Error: " <<  SDL_GetError() << std::endl;
    exit(1);
  }
  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
    std::cout << "IMG_Init " << SDL_GetError();
    SDL_Quit();
    exit(1);
  }
  if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "2" ) ) {
    std::cerr << "Couldn't enable anisotropic texture filtering, trying linear.\n" ;
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) {
      std::cerr << "Couldn't enable linear texture filtering, using nearest.\n" ;
    }
  }

  if (TTF_Init() != 0){
    std::cerr << "[SbWindow::initialize] Error in TTF_init: " << SDL_GetError() << std::endl;
    SDL_Quit();
    exit(1);
  }
}


void sdl_quit()
{
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
}

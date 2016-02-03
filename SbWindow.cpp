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
 if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    std::cerr << "SDL could not initialize! SDL_Error: " <<  SDL_GetError() << std::endl;
    exit(1);
  }
  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
    std::cout << "IMG_Init " << SDL_GetError();
    SDL_Quit();
    exit(1);
  }
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
  SDL_SetRenderDrawColor( renderer_, 0xFF, 0xFF, 0xFF, 0xFF );

  if (TTF_Init() != 0){
    std::cerr << "[SbWindow::initialize] Error in TTF_init: " << SDL_GetError() << std::endl;
    SDL_Quit();
    exit(1);
  }

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

  IMG_Quit();
  SDL_Quit();
}



void
SbWindow::handle_event(const SDL_Event& event)
{
}

/*! \file SbTexture.cpp
  part of SDL2-basic
  author: Ulrike Hager
 */

#include <string>
#include <stdexcept>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "SbTexture.h"



/*! SbTexture implementation
 */
SbTexture::~SbTexture()
{
  clear();
}


void
SbTexture::clear()
{
  if ( texture_ )
    {
      SDL_DestroyTexture( texture_ );
      texture_ = nullptr;
      width_ = 0;
      height_ = 0;
    }
}



SbTexture*
SbTexture::from_file(SDL_Renderer* renderer, const std::string& filename, int width, int height )
{
  texture_ = IMG_LoadTexture(renderer, filename.c_str());
  
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
SbTexture::from_rectangle( SDL_Renderer* renderer, int width, int height, const SDL_Color& color )
{
  clear();
  texture_ = SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, width, height);
  if (texture_ == nullptr) 
    throw std::runtime_error("Failed to create texture " + std::string( SDL_GetError() ));
  
  SDL_RenderClear(renderer);
  SDL_SetRenderTarget(renderer, texture_);
  SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, color.a );
  SDL_Rect sourceRect = {0,0,width,height};
  int check = SDL_RenderFillRect( renderer, &sourceRect );
  if ( check != 0 )
    throw std::runtime_error("Couldn't render rectangle: " + std::string( SDL_GetError() ));
  SDL_SetRenderTarget(renderer, nullptr);
  SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0x00, 0xFF );
  SDL_RenderClear(renderer);

  width_ = width;
  height_ = height;
  return this;
}



SbTexture*
SbTexture::from_text( SDL_Renderer* renderer, const std::string& text, TTF_Font* font, const SDL_Color& color )
{
  clear();
  SDL_Surface *surf = TTF_RenderText_Solid(font, text.c_str(), color);
  if (surf == nullptr)
    throw std::runtime_error("Failed to create surface from text: " + std::string( SDL_GetError() ));

  texture_ = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_FreeSurface(surf);
  if (texture_ == nullptr) 
    throw std::runtime_error("Failed to create texture " + std::string( SDL_GetError() ));

  SDL_QueryTexture( texture_, nullptr, nullptr, &width_, &height_ );
  return this;
}


void
SbTexture::render( SDL_Renderer* renderer, int x, int y, SDL_Rect* sourceRect)
{
  SDL_Rect destRect = { x, y, width_, height_ };
  render( renderer, &destRect, sourceRect);
}



void
SbTexture::render( SDL_Renderer* renderer, SDL_Rect *bounding_rect, SDL_Rect* sourceRect)
{
  SDL_RenderCopy( renderer, texture_, sourceRect, bounding_rect );
}

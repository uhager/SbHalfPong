/*! \file SbTexture.cpp
  part of SDL2-basic
  author: Ulrike Hager
 */

#include <string>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "SbTexture.h"


extern SDL_Renderer* gRenderer;

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
SbTexture::createFromText( const std::string& text, TTF_Font* font, const SDL_Color& color )
{
#ifdef DEBUG
  std::cout << "[SbTexture::createFromText]" << std::endl;
#endif
  
  clear();
  SDL_Surface *surf = TTF_RenderText_Solid(font, text.c_str(), color);
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


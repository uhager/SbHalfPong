/*! \file SbTexture.h
  part of SDL2-basic
  author: Ulrike Hager
 */

#ifndef SBTEXTURE_H
#define SBTEXTURE_H

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


class SbTexture
{
public:
  SbTexture() = default;
  ~SbTexture();

  SbTexture* from_file( SDL_Renderer* renderer, const std::string& filename, int width = 0, int height = 0);
  SbTexture* from_rectangle( SDL_Renderer* renderer, int width, int height, const SDL_Color& color );
  SbTexture* from_text( SDL_Renderer* renderer, const std::string& text, TTF_Font* font, const SDL_Color& color );
  void clear();
  void render( SDL_Renderer* renderer, int x, int y, SDL_Rect* sourceRect = nullptr);
  void render( SDL_Renderer* renderer, SDL_Rect *bounding_rect, SDL_Rect* sourceRect = nullptr);
  int getWidth(){ return width_; }
  int getHeight(){ return height_;}

private:
  SDL_Texture* texture_ = nullptr;
  int width_ = 0;
  int height_ = 0 ;
};


#endif // SBTEXTURE_H

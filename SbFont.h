// part of sdl-basic
// author: Ulrike Hager

/*! \file SbFont.h
  \brief SbFont class holds font information.
*/

#ifndef SBFONT_H
#define SBFONT_H

#include <string>
#include <stdexcept>
#include <memory>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>



class SbFont
{
 public:
  SbFont() {}
  SbFont(std::string fontfile, int fontsize) {
    TTF_Font* tmp_font = TTF_OpenFont(fontfile.c_str(), fontsize);
    if ( !tmp_font )
      throw std::runtime_error( "TTF_OpenFont: " + std::string( TTF_GetError() ) );
    font_ = std::shared_ptr<TTF_Font>(tmp_font, delete_font );
  }

  ~SbFont() {}
  SbFont(const SbFont& toCopy)
    : color(toCopy.color), font_(toCopy.font_)
    { }
  SbFont& operator=(const SbFont& toCopy)  {
    color = toCopy.color;
    font_ = toCopy.font_;
    return *this;
  }
  
  SDL_Color color = {210, 160, 10, 0};

  std::shared_ptr<TTF_Font> font() {return font_;}

 private:
  static void delete_font( TTF_Font* ft) {
    if ( ft ) {
      TTF_CloseFont( ft );
    }
  }

  std::shared_ptr<TTF_Font> font_ = nullptr;


};


#endif  /* SBFONT_H */



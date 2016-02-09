/*! \file SbMessage.h
  part of SDL2-basic
  author: Ulrike Hager
 */

#ifndef SBMESSAGE_H
#define SBMESSAGE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "SbObject.h"


class SbMessage : public SbObject
{
public:
  SbMessage(double x, double y, double width, double height);
  ~SbMessage();

  void set_font(TTF_Font* font){ font_ = font;}
  void set_text(std::string text);

 protected: 
  TTF_Font* font_ = nullptr;

 private:
  SDL_Color color_{210, 160, 10, 0};
};


#endif  //  SBMESSAGE_H

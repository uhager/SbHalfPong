/*! \file SbMessage.h
  part of SDL2-basic
  author: Ulrike Hager
 */

#ifndef SBMESSAGE_H
#define SBMESSAGE_H

#include <deque>
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



class SbFpsDisplay : public SbMessage
{
 public:
  SbFpsDisplay(TTF_Font *font, double x = 0, double y = 0, double width = 0.06, double height= 0.035);
  void set_number_frames( unsigned int n );
  void update();
  
 private:
  unsigned int n_frames_ = 250;
  double sum_ = 0;
  std::deque<double> times_;
  
};


#endif  //  SBMESSAGE_H


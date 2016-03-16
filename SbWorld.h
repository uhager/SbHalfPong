#ifndef SBWORLD_H
#define  SBWORLD_H

#include <SDL2/SDL.h>

#include "SbWindow.h"

class SbWorld
{
 public:
  SbWorld(int width, int height)
    : width_(width), height_(height)   {
    rel_width_ = width_/window->width();
    rel_height_ = height_/window->height();
  }
  SbWorld() = default;

  void handle_event(const SDL_Event& event) {
    if ( window->new_size() ) {
      width_ = static_cast<int>(window->width() * rel_width_); 
      height_ = static_cast<int>(window->height() * rel_height_); 
    }
  }
    
  static SbWindow* window ;

  bool new_size() { return new_size_; }
  int width() const {return width_;}
  int height() const { return height_; }
  
 private:
  bool new_size_ = false;
  int width_ = 800;
  int height_ = 600;
  double rel_width_ = 1.0;
  double rel_height_ = 1.0;
};



#endif  //  SBWORLD_H

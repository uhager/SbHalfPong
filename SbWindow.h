/*! \file SbWindow.h
  part of SDL2-basic
  author: Ulrike Hager
 */

#ifndef SBWINDOW_H
#define SBWINDOW_H

#include <string>
#include <array>

#include <SDL2/SDL.h>


class SbWindow
{
 public:
  SbWindow() = default;
  ~SbWindow();
  
  void close();
  void handle_event(const SDL_Event& event);
  int height(){return height_;}
  void initialize(std::string title, int width, int height);
  SDL_Renderer* renderer() {return renderer_;}
  int width() {return width_;}
  bool new_size() { return new_size_; }
  
 private:
  SDL_Renderer* renderer_ = nullptr;
  SDL_Window* window_ = nullptr;
  int width_;
  int height_;
  SDL_Color background_color_;  
  bool new_size_ = false;
};



#endif   //  SBWINDOW_H

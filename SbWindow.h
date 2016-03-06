/*! \file SbWindow.h
  part of SDL2-basic
  author: Ulrike Hager
 */

#ifndef SBWINDOW_H
#define SBWINDOW_H

#include <string>
#include <array>
#include <memory>
#include <iostream>

#include <SDL2/SDL.h>


struct DeleteWindow
{
  void operator()(SDL_Window* win) const{
#ifdef DEBUG
    std::cout << "[DeleteWindow]" << std::endl;
#endif
    if (win){
      SDL_DestroyWindow(win);
      win = nullptr;
    }
  }
};


struct DeleteRenderer
{
  void operator()(SDL_Renderer* ren) const{
#ifdef DEBUG
    std::cout << "[DeleteRenderer]" << std::endl;
#endif
    if (ren){
      SDL_DestroyRenderer(ren);
      ren = nullptr;
    }
  }
};



class SbWindow
{
 public:
  SbWindow(std::string title, int width, int height);
  SbWindow(const SbWindow&) = delete;
  SbWindow& operator=(const SbWindow&) = delete;
  ~SbWindow();
  
  void close();
  void handle_event(const SDL_Event& event);
  int height(){return height_;}
  SDL_Renderer* renderer() {return renderer_.get();}
  int width() {return width_;}
  bool new_size() { return new_size_; }
  
 private:
  std::unique_ptr<SDL_Renderer, DeleteRenderer> renderer_ = nullptr;
  std::unique_ptr<SDL_Window, DeleteWindow> window_ = nullptr;
  int width_;
  int height_;
  SDL_Color background_color_;  
  bool new_size_ = false;
  bool is_fullscreen = false;
};

void sdl_init();
void sdl_quit();


#endif   //  SBWINDOW_H

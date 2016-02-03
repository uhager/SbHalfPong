/*! \file SbWindow.h
  part of SDL2-basic
  author: Ulrike Hager
 */


#include <string>

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

 
 private:
  SDL_Renderer* renderer_ = nullptr;
  SDL_Window* window_ = nullptr;
  int width_;
  int height_;
  SDL_Color background_color_;  
};


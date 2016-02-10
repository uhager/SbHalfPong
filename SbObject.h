/*! \file SbObject.h
  part of SDL2-basic
  author: Ulrike Hager
 */

#ifndef SBOBJECT_H
#define SBOBJECT_H

#include <vector>
#include <iostream>
#include <array>

#include <SDL2/SDL.h>

#include "SbTimer.h"

class SbTexture;
class SbWindow;

class SbObject
{
public:
  SbObject() = default;
  SbObject(int x, int y, int width, int height);
  SbObject(double x, double y, double width, double height);
  virtual ~SbObject();

static SbWindow* window;

  virtual void handle_event(const SDL_Event& event);
  virtual int move( std::vector<SbObject*> objects_to_hit );
  virtual int move( );
  virtual void render();
  std::array<double,4> bounding_box() { return bounding_box_;};
  SDL_Rect bounding_rect() {return bounding_rect_;}
  bool is_inside(int x, int y);
  void move_bounding_box();
  std::string name(){return name_;}
  std::ostream& print_dimensions(std::ostream& os); 
  void start_timer() {timer_.start();}
  Uint32 time() {return timer_.get_time();}
  virtual void was_hit();
  
protected:
  SDL_Rect bounding_rect_ = {70, 200, 20, 70} ;
  //! location and size in terms of window width and height
  std::array<double,4> bounding_box_ = { {0.5, 0.5, 0.05, 0.05} };
  /*! velocities are in ms to the screen. so smaller is actually faster. Should maybe rename that...
   */
  double velocity_y_ = 0;
  double velocity_x_ = 0;
  double velocity_ = 0;
  SbTexture* texture_ = nullptr;
  SDL_Color color = {210, 160, 10, 0};
  SbTimer timer_;
  std::string name_ = "other"; 
};



#endif  // SBOBJECT_H

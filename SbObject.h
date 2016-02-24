/*! \file SbObject.h
  part of SDL2-basic
  author: Ulrike Hager
 */

#ifndef SBOBJECT_H
#define SBOBJECT_H

#include <vector>
#include <iostream>
#include <array>
#include <memory>

#include <SDL2/SDL.h>

#include "SbTimer.h"

class SbTexture;
class SbWindow;

enum class SbHitPosition {
  none, top, bottom, left, right
    };


struct SbRectangle
{
  SbRectangle(double xn, double yn, double wn, double hn)
  : x(xn), y(yn), w(wn), h(hn)
  {}
  SbRectangle() = default;
  double x = 0;
  double y = 0;
  double w = 0;
  double h = 0;
};


  
class SbObject
{
public:
  SbObject() = default;
  SbObject(int x, int y, int width, int height);
  SbObject(double x, double y, double width, double height);
  /* SbObject(SbObject&& toMove) */
  /*   : texture_(toMove.texture_) */
  /*   {} */
  /* SbObject(const SbObject& toCopy) = default; */
  virtual ~SbObject();

static SbWindow* window;

 SbHitPosition check_hit(const SbObject& toHit);
 virtual void handle_event(const SDL_Event& event);
  virtual int move( );
  virtual void render() ;
  virtual void render(const SDL_Rect &camera);
  SbRectangle bounding_box() { return bounding_box_;};
  SDL_Rect bounding_rect() const {return bounding_rect_;}
  bool has_mouse(){return has_mouse_;}
  bool is_inside(int x, int y);
  void move_bounding_box();
  std::string name(){return name_;}
  std::ostream& print_dimensions(std::ostream& os); 
  void start_timer() {timer_.start();}
  void stop_timer() {timer_.stop();}
  void set_color( int red, int green, int blue );
  Uint32 time() {return timer_.get_time();}
  bool timer_started() { return timer_.started(); }
  virtual void was_hit();
  int width() const { return bounding_rect_.w;}
  int height() const { return bounding_rect_.h;}
  int pos_x() const { return bounding_rect_.x;}
  int pos_y() const { return bounding_rect_.y;}
  
protected:
  SDL_Rect bounding_rect_ = {70, 200, 20, 70} ;
  //! location and size in terms of window width and height
  SbRectangle bounding_box_ = {0.5, 0.5, 0.05, 0.05} ;
  /*! velocities are in ms to the screen. so smaller is actually faster. Should maybe rename that...
   */
  double velocity_y_ = 0;
  double velocity_x_ = 0;
  double velocity_ = 0;
  SbTexture* texture_ = nullptr;
  SDL_Color color_ = {210, 160, 10, 0};
  SbTimer timer_;
  std::string name_ = "other";
  bool has_mouse_ = false;
  bool render_me_ = true;
};



#endif  // SBOBJECT_H

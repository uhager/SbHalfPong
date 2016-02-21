/*! \file SbObject.cpp
  part of SDL2-basic
  author: Ulrike Hager
 */

#include <iostream>
//#include <cmath>

#include "SbTexture.h"
#include "SbWindow.h"

#include "SbObject.h"



SbObject::SbObject(int x, int y, int width, int height)
{
  texture_ = new SbTexture();
  bounding_rect_ = {x,y,width, height};
  bounding_box_[0] = double(bounding_rect_.x)/window->width();
  bounding_box_[1] = double(bounding_rect_.y)/window->height();
  bounding_box_[2] = double(bounding_rect_.w)/window->width();
  bounding_box_[3] = double(bounding_rect_.h)/window->height();
}



SbObject::SbObject(double x, double y, double width, double height)
{
  texture_ = new SbTexture();
  bounding_box_ = {x,y,width, height};
  bounding_rect_.x = static_cast<int>(x * window->width() );
  bounding_rect_.y = static_cast<int>(y * window->height() );
  bounding_rect_.w = static_cast<int>(width * window->width() );
  bounding_rect_.h = static_cast<int>(height * window->height() );
}



SbObject::~SbObject()
{
  delete texture_;
  texture_ = nullptr;
}



SbHitPosition
SbObject::check_hit(const SbObject& toHit)
{
  SbHitPosition result = SbHitPosition::none;
  const SDL_Rect& hit_box = toHit.bounding_rect();
  bool in_xrange = false, in_yrange = false, x_hit_left = false, x_hit_right = false, y_hit_top = false, y_hit_bottom = false ;

  if ( bounding_rect_.x + bounding_rect_.w/2 >= hit_box.x &&
       bounding_rect_.x + bounding_rect_.w/2 <= hit_box.x + hit_box.w )
    in_xrange = true;
      
  if ( bounding_rect_.y + bounding_rect_.h/2 >= hit_box.y  &&
       bounding_rect_.y + bounding_rect_.h/2 <= hit_box.y + hit_box.h)
     in_yrange = true;

  if ( bounding_rect_.x + bounding_rect_.w >= hit_box.x               &&
       bounding_rect_.x                   <= hit_box.x + hit_box.w ) {
    if ( bounding_rect_.x > hit_box.x ) 
      x_hit_right = true;
    else if ( bounding_rect_.x + bounding_rect_.w < hit_box.x + hit_box.w)
      x_hit_left = true;
  }
  if ( bounding_rect_.y + bounding_rect_.h >= hit_box.y               &&
       bounding_rect_.y                   <= hit_box.y + hit_box.h ) {
    if ( bounding_rect_.y > hit_box.y ) 
      y_hit_bottom = true;
    else if ( bounding_rect_.y + bounding_rect_.h < hit_box.y + hit_box.h )
      y_hit_top = true;
  }
  
  if ( x_hit_left && in_yrange ) 
    result = SbHitPosition::left;
  else if (x_hit_right && in_yrange)
    result = SbHitPosition::right;
  else if (y_hit_top && in_xrange)
    result = SbHitPosition::top;
  else if (y_hit_bottom && in_xrange)
    result = SbHitPosition::bottom;
  return result;
}



void
SbObject::handle_event(const SDL_Event& event)
{
  if ( window->new_size() ) {
    // update bounding_box_ origin in case object moved
   // velocity_x_ = static_cast<int>(scale[0] * velocity_x_);
    // velocity_y_ = static_cast<int>(scale[1] * velocity_y_);;
    // velocity_  = static_cast<int>( (scale[0] + scale[1]) * velocity_);;
    bounding_rect_.x = static_cast<int>(window->width() * bounding_box_[0]);
    bounding_rect_.y = static_cast<int>(window->height() * bounding_box_[1]);
    bounding_rect_.w = static_cast<int>(window->width() * bounding_box_[2]); 
    bounding_rect_.h = static_cast<int>(window->height() * bounding_box_[3]); 
  }
}



bool
SbObject::is_inside(int x, int y)
{
  has_mouse_ = true;
  if ( x < bounding_rect_.x || x > bounding_rect_.x + bounding_rect_.w ||
       y < bounding_rect_.y || y > bounding_rect_.y + bounding_rect_.h )
    has_mouse_ = false;
  return has_mouse_;
}


int
SbObject::move( )
{
#ifdef DEBUG
  std::cout << "[SbObject::move]" << std::endl;
#endif
  return 0;
}



void
SbObject::move_bounding_box()
{
  bounding_box_[0] = double(bounding_rect_.x) / window->width();
  bounding_box_[1] = double(bounding_rect_.y) / window->height();
}
  


std::ostream&
SbObject::print_dimensions(std::ostream& os)
{
  os << "window = " << window->height() << "x" << window->width()
     << " bounding box: " << bounding_box_[0] << "," << bounding_box_[1]
     << " - " << bounding_box_[2] << "x" << bounding_box_[3]
     << " ;bounding rect: " << bounding_rect_.x << "," << bounding_rect_.y
     << " - "  << bounding_rect_.w << "x" << bounding_rect_.h;
  return os;
}



void
SbObject::render()
{
  if (render_me_ && texture_) texture_->render( window->renderer(), &bounding_rect_ );
}


void
SbObject::render( const SDL_Rect &camera )
{
  if (render_me_ && texture_) {
    SDL_Rect camera_adjusted = bounding_rect_;
    camera_adjusted.x -= camera.x;
    camera_adjusted.y -= camera.y;
    texture_->render( window->renderer(), &camera_adjusted );
  }
}


void
SbObject::set_color( int red, int green, int blue )
{
  color_.r = red;
  color_.g = green;
  color_.b = blue;
}


void
SbObject::was_hit()
{
#ifdef DEBUG
  std::cout << "[SbObject::was_hit]" << std::endl;
#endif
}

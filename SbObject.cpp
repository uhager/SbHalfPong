/*! \file SbObject.cpp
  part of SDL2-basic
  author: Ulrike Hager
 */

#include <iostream>
#include <memory>

#include "SbTexture.h"
#include "SbWindow.h"

#include "SbObject.h"



SbObject::SbObject(int x, int y, int width, int height)
{
  texture_ = std::make_shared<SbTexture>();
  bounding_rect_ = {x,y,width, height};
  bounding_box_ = { double(bounding_rect_.x)/window->width()
		    , double(bounding_rect_.y)/window->height()
		    , double(bounding_rect_.w)/window->width()
		    , double(bounding_rect_.h)/window->height() };
}



SbObject::SbObject(double x, double y, double width, double height)
{
  texture_ = std::make_shared<SbTexture>();
  bounding_box_ = {x,y,width, height};
  bounding_rect_.x = static_cast<int>(x * window->width() );
  bounding_rect_.y = static_cast<int>(y * window->height() );
  bounding_rect_.w = static_cast<int>(width * window->width() );
  bounding_rect_.h = static_cast<int>(height * window->height() );
}



SbObject::SbObject( SbRectangle bounding_box)
  : bounding_box_(bounding_box)
{
  texture_ = std::make_shared<SbTexture>();
  bounding_rect_.x = static_cast<int>(bounding_box_.x * window->width() );
  bounding_rect_.y = static_cast<int>(bounding_box_.y * window->height() );
  bounding_rect_.w = static_cast<int>(bounding_box_.w * window->width() );
  bounding_rect_.h = static_cast<int>(bounding_box_.h * window->height() );
}



void
SbObject::center_camera(SDL_Rect& camera, int w, int h) 
{
  camera.w = window->width();
  camera.h = window->height();
  camera.x = pos_x() + width()/2 - camera.w/2;
  camera.y = pos_y() + height()/2 - camera.h/2;
  if ( camera.x < 0 )
      camera.x = 0;
  else if ( camera.x > w - camera.w )
      camera.x = w - camera.w;
  
  if ( camera.y < 0 )
      camera.y = 0;
  else if( camera.y > h - camera.h )
    camera.y = h - camera.h;
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
SbObject::update_size()
{
    bounding_rect_.x = static_cast<int>(window->width() * bounding_box_.x);
    bounding_rect_.y = static_cast<int>(window->height() * bounding_box_.y);
    bounding_rect_.w = static_cast<int>(window->width() * bounding_box_.w); 
    bounding_rect_.h = static_cast<int>(window->height() * bounding_box_.h); 
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
  bounding_box_.x = double(bounding_rect_.x) / window->width();
  bounding_box_.y = double(bounding_rect_.y) / window->height();
}
  

void
SbObject::move_bounding_rect()
{
  bounding_rect_.x = double(bounding_box_.x * window->width() );
  bounding_rect_.y = double(bounding_box_.y * window->height() );
}
  


std::ostream&
SbObject::print_dimensions(std::ostream& os)
{
  os << "window = " << window->height() << "x" << window->width()
     << " bounding box: " << bounding_box_.x << "," << bounding_box_.y
     << " - " << bounding_box_.w << "x" << bounding_box_.h
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

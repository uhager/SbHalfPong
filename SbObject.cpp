/*! \file SbObject.cpp
  part of SDL2-basic
  author: Ulrike Hager
 */

#include <iostream>
#include <cmath>

#include "SbTexture.h"
#include "SbWindow.h"

#include "SbObject.h"



SbObject::SbObject(int x, int y, int width, int height)
{
  bounding_rect_ = {x,y,width, height};
  bounding_box_[0] = double(bounding_rect_.x)/window->width();
  bounding_box_[1] = double(bounding_rect_.y)/window->height();
  bounding_box_[2] = double(bounding_rect_.w)/window->width();
  bounding_box_[3] = double(bounding_rect_.h)/window->height();
}



SbObject::SbObject(double x, double y, double width, double height)
{
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




void
SbObject::handle_event(const SDL_Event& event)
{
#ifdef DEBUG
  std::cout << "[SbObject::handle_event]" << std::endl;
#endif
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



void
SbObject::move( std::vector<SbObject*> objects_to_hit )
{
#ifdef DEBUG
  std::cout << "[SbObject::move]" << std::endl;
#endif
}



void
SbObject::move( )
{
#ifdef DEBUG
  std::cout << "[SbObject::move]" << std::endl;
#endif
}



void
SbObject::render()
{
  if (texture_) texture_->render( window->renderer(), &bounding_rect_ );
}



void
SbObject::move_bounding_box()
{
  bounding_box_[0] = double(bounding_rect_.x) / window->width();
  bounding_box_[1] = double(bounding_rect_.y) / window->height();
}
  
void
SbObject::was_hit()
{
#ifdef DEBUG
  std::cout << "[SbObject::was_hit]" << std::endl;
#endif
}

/*! \file SbObject.cpp
  part of SDL2-basic
  author: Ulrike Hager
 */

#include <iostream>

#include "SbTexture.h"
#include "SbWindow.h"

#include "SbObject.h"



SbObject::SbObject(int x, int y, int width, int height)
{
  bounding_box_ = {x,y,width, height};
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
}



void
SbObject::move( std::vector<SbObject*> objects_to_hit )
{
#ifdef DEBUG
  std::cout << "[SbObject::move]" << std::endl;
#endif
}



void
SbObject::render()
{
  if (texture_) texture_->render( window->renderer(), bounding_box_.x, bounding_box_.y );
}


  
void
SbObject::was_hit()
{
#ifdef DEBUG
  std::cout << "[SbObject::was_hit]" << std::endl;
#endif
}

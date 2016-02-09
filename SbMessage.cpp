/*! \file SbMessage.cpp
  part of SDL2-basic
  author: Ulrike Hager
 */


#include <stdexcept>

#include "SbTexture.h"
#include "SbWindow.h"

#include "SbMessage.h"

SbMessage::SbMessage(double x, double y, double width, double height)
  : SbObject(x,y,width,height)
{
}



void
SbMessage::set_text(std::string message)
{
  if ( !font_)
    throw std::runtime_error( "[SbMessage::set_text] no font. Call set_font before setting the text." );

  texture_->from_text( window->renderer(), message, font_, color_);
}



SbMessage::~SbMessage()
{
  font_ = nullptr;
}



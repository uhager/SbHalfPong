/*! \file SbObject.h
  part of SDL2-basic
  author: Ulrike Hager
 */

#ifndef SBOBJECT_H
#define SBOBJECT_H

#include <vector>

#include <SDL2/SDL.h>

#include "SbTimer.h"

class SbTexture;

class SbObject
{
public:
  SbObject() = default;
  SbObject(int x, int y, int width, int height);
  virtual ~SbObject();
  virtual void handle_event(const SDL_Event& event);
  virtual void move( std::vector<SbObject*> objects_to_hit );
  void render();
  SDL_Rect get_bounding_box() {return bounding_box_;}
  virtual void was_hit();
  
protected:
  SDL_Rect bounding_box_ = {70, 200, 20, 70}; 
  double y_velocity_ = 0;
  double x_velocity_ = 0;
  double velocity_ = 0.7;
  SbTexture* texture_ = nullptr;
  SDL_Color color = {210, 160, 10, 0};
  SbTimer timer_;
};



#endif  // SBOBJECT_H

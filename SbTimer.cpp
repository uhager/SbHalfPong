/*! \file SbTimer.cpp
  part of SDL2-basic
  author: Ulrike Hager
 */


#include "SbTimer.h"

/*! SbTimer implementation
 */
void
SbTimer::start()
{
  started_ = true;
  startTime_ = SDL_GetTicks();
}


void
SbTimer::stop()
{
  started_ =false;
  // When stopped, the timer will return the time interval between start and stop. This is saved in startTime_ until the timer is restarted.
  startTime_ = SDL_GetTicks() - startTime_;
}


Uint32
SbTimer::get_time()
{
  Uint32 time = 0;
  if ( started_ )
    time = SDL_GetTicks() - startTime_;
  else
    time = startTime_;
  return time;
}


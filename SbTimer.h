/*! \file SbTimer.h
  part of SDL2-basic
  author: Ulrike Hager
 */


#include <SDL2/SDL.h>


class SbTimer
{
public:
  SbTimer() = default;
  void start();
  void stop();
  /*! All times in ms
   */
  Uint32 getTime();
  bool started(){ return started_ ;}

private:
  Uint32 startTime_ = 0;
  bool started_ = false;
};




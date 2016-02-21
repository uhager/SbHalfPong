/* SbMaze.h
author: Ulrike Hager
*/

#ifndef SBMAZE_H
#define SBMAZE_H


#include <string>


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "SbObject.h"
#include "SbMessage.h"


class Ball;
class Tile;
class Goal;


class Ball : public SbObject
{
public:
  Ball();

  void center_camera(SDL_Rect& camera);
  bool check_goal(const Goal& goal);
  void handle_event(const SDL_Event& event);
  int move(const std::vector<std::unique_ptr<SbObject>>& level);
  //  void render();
  /*! Reset after goal.
   */
  void reset();
  static Uint32 resetball(Uint32 interval, void *param );
  void set_momentum_loss(double ml) {momentum_loss_ = ml;}
  
private:
  int goal_ = 0;
  //!  momentum lost in collision = (1-momentum_loss_) * momentum before collision
  double momentum_loss_ = 0.95;
};



class Tile : public SbObject
{
 public:
  Tile(int x, int y, int width, int height);
};



class Goal : public SbObject
{
 public:
  Goal(int x, int y, int width, int height);
};

#endif  // SBMAZE_H

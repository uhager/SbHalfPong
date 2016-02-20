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



class Ball : public SbObject
{
public:
  Ball();

  void center_camera(SDL_Rect& camera);
  SbHitPosition check_hit(const std::unique_ptr<Tile>& tile);
  void handle_event(const SDL_Event& event);
  /*! \retval 1 if ball in goal
    \retval 0 else
   */
  int move(const std::vector<std::unique_ptr<Tile>>& level);
  //  void render();
  /*! Reset after goal.
   */
  void reset();
  static Uint32 resetball(Uint32 interval, void *param );
  void set_momentum_loss(double ml) {momentum_loss_ = ml;}
  
private:
  //  momentum lost in collision = (1-momentum_loss_) * momentum before collision
  double momentum_loss_ = 0.95;
};



class Tile : public SbObject
{
 public:
  Tile(int x, int y, int width, int height);
};

#endif  // SBMAZE_H

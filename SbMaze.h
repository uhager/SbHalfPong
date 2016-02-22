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
class Level;

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



class Level
{
 public:
  Level(int num, TTF_Font* font );
  ~Level() = default;
  
  void create_level(int num);
  void start_timer(){ timer_.start(); }
  void stop_timer(){ timer_.stop(); }

  Goal const& goal() const {return goal_;}
  std::vector<std::unique_ptr<SbObject>> const& tiles() const {return tiles_; }
  unsigned width() { return width_; }
  unsigned height() {return height_; }
  void render(const SDL_Rect &camera);
  unsigned level_number() { return level_num_; }
  
 private:
  unsigned width_;
  unsigned height_;
  unsigned level_num_ = 0;
  Goal goal_;
  std::vector<std::unique_ptr<SbObject>> tiles_;
  SbTimer timer_;
  SbMessage time_message_;
  
};




class Maze
{
 public:
  Maze();
  ~Maze();
  Maze(const Maze&)  = delete ;
  Maze& operator=(const Maze& toCopy) = delete;

  void initialize();
  void reset();
  static Uint32 reset_game(Uint32 interval, void *param );
  void run();
  SbWindow* window() {return &window_; }
  
 private:
  std::unique_ptr<Ball> ball_;
  std::unique_ptr<Level> level_ = nullptr;
  SbWindow window_;
  SDL_Rect camera_;
  TTF_Font *font_;
  std::unique_ptr<SbFpsDisplay> fps_display_ = nullptr;
};

#endif  // SBMAZE_H

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
  bool goal_ = false;
  //!  momentum lost in collision = (1-momentum_loss_) * momentum before collision
  double momentum_loss_ = 0.9;
  double velocity_max_ = 1.0/800.0;
};



class Tile : public SbObject
{
 public:
  Tile(int x, int y, int width, int height);
  Tile( SbRectangle bounding_box );
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
  
  void create_level(uint32_t num);
  void start_timer(){ time_message_.start_timer(); }
  void stop_timer(){ time_message_.stop_timer(); }
  Uint32 time() { return time_message_.time(); }
    
  Goal const& goal() const {return *goal_;}
  std::vector<std::unique_ptr<SbObject>> const& tiles() const {return tiles_; }
  uint32_t width() { return width_; }
  uint32_t height() {return height_; }
  void render(const SDL_Rect &camera);
  uint32_t level_number() { return level_num_; }
  void handle_event(const SDL_Event& event);
  
 private:
  uint32_t width_;
  uint32_t height_;
  uint32_t level_num_ = 0;
  std::unique_ptr<Goal> goal_ = nullptr;
  std::vector<std::unique_ptr<SbObject>> tiles_;
  SbMessage time_message_;
};


class HighScore : public SbMessage
{
 public:
  HighScore(TTF_Font *font, std::string filename = "maze.save");
  bool check_highscore(uint32_t level, Uint32 score);
  std::vector<Uint32> highscores() { return highscores_; }
  void write_highscores( );
  std::vector<Uint32> read_highscores( );
  void write_highscore();

 private:
  std::vector<Uint32> highscores_;
  std::string savefile_;
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
  bool in_goal_ = false;
  uint32_t current_level_ = 0;
  SbWindow window_;
  SDL_Rect camera_;
  TTF_Font *font_;
  std::unique_ptr<SbFpsDisplay> fps_display_ = nullptr;
  SbTimer reset_timer_;
  std::unique_ptr<HighScore> highscore_ = nullptr;
};


struct LevelCoordinates
{
  LevelCoordinates(std::vector<SbRectangle> t, SbRectangle g)
  : tiles(t),goal(g)
  {}
  std::vector<SbRectangle> tiles;
  SbRectangle goal;
};


////////////////////
////// levels //////
////////////////////
std::vector<LevelCoordinates> levels;

std::vector<SbRectangle> lev0 = {{0,0,1.0,0.05}, {0.95,0.0,0.05,1.0}, {0.0,0.,0.05,1.0}, {0.0, 0.95, 1.0, 0.05}  /* outer boxes */
				 , {0.45,0.45,0.1,0.1},{0.35,0.35,0.1,0.1}, {0.55,0.35,0.1,0.1}, {0.55,0.55,0.1,0.1}, {0.35,0.55,0.1,0.1}    /* central boxes */ 
				 ,{ 0.85, 0.4, 0.03, 0.53 }	/* barrier next to goal*/				      
	};
SbRectangle goal0 = {0.4, 0.48, 0.03, 0.03};

std::vector<SbRectangle> lev1 = {{0,0,1.0,0.03}, {0.97,0.0,0.03,1.0}, {0.0,0.,0.03,1.0}, {0.0, 0.97, 1.0, 0.03}  /* outer boxes */
				 , {0.15,0.85,0.18,0.03}, {0.4, 0.85, 0.52, 0.03} /* lower horiz. bars */
				 , {0.2, 0.2, 0.03, 0.6}, {0.6, 0.0, 0.03, 0.4}  /* vert. bars */
				 , {0.45, 0.6, 0.55, 0.03}   /* middle horiz. bar */
};
SbRectangle goal1 = {0.85, 0.1, 0.03, 0.03};

////////////////////
//// levels end ////
////////////////////



#endif  // SBMAZE_H

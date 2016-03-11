#ifndef SBPLATFORMER_H
#define SBPLATFORMER_H

#include <memory>
#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "SbMessage.h"
#include "SbWindow.h"
#include "SbObject.h"
#include "SbFont.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int LEVEL_WIDTH = 2000;
const int LEVEL_HEIGHT = 1500;
const int CONTROLLER_DEADZONE = 6000;
const std::string name = "Platformer";

class Exit;
class Platform;
class Level;
class Platformer;

enum class ControlDir {
  none, left, right, up, down
};



class Player : public SbObject
{
 public:
  Player();

  bool check_exit(const Exit& goal);
  void handle_event(const SDL_Event& event);
  int move(const std::vector<std::unique_ptr<SbObject>>& level);
  //  void render();
  /*! Reset after goal.
   */
  void reset();

 private:
  bool exit_ = false;
  double velocity_max_ = 1.0/800.0;
  double velocity_jump_ = 1.0/800.0;
  bool on_surface_ = true;
};



class Platform : public SbObject
{
 public:
    Platform(int x, int y, int width, int height);
};


class Exit : public SbObject
{
 public:
  Exit(int x, int y, int width, int height);

};


class Level
{
 public:
  Level(uint32_t num);

  void create_level(uint32_t num);
   Exit const& exit() const {return *exit_;}
  std::vector<std::unique_ptr<SbObject>> const& platforms() const {return platforms_; }
  uint32_t width() { return width_; }
  uint32_t height() {return height_; }
  void render(const SDL_Rect &camera);
  uint32_t level_number() { return level_num_; }
  //  void handle_event(const SDL_Event& event);
  
 private:
  uint32_t width_ = LEVEL_WIDTH;
  uint32_t height_ = LEVEL_HEIGHT;
  uint32_t level_num_ = 0;
  std::unique_ptr<Exit> exit_ = nullptr;
  std::vector<std::unique_ptr<SbObject>> platforms_;
};



class Platformer
{
 public:
  Platformer();
  ~Platformer();
  Platformer(const Platformer&)  = delete ;
  Platformer& operator=(const Platformer& toCopy) = delete;

  void initialize();
  void reset();
  static Uint32 reset_game(Uint32 interval, void *param );
  void run();
  SbWindow* window() {return &window_; }
  
 private:
  std::unique_ptr<Player> player_;
  std::unique_ptr<Level> level_ = nullptr;
  SDL_GameController* game_controller_ = nullptr;
  bool in_exit_ = false;
  uint32_t current_level_ = 0;
  SbWindow window_{name, SCREEN_WIDTH, SCREEN_HEIGHT};
  SDL_Rect camera_;
  std::unique_ptr<SbFpsDisplay> fps_display_ = nullptr;
  SbTimer reset_timer_;

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

std::vector<SbRectangle> lev0 = {{0,0,1.0,0.03}, {0.97,0.0,0.03,1.0}, {0.0,0.0,0.03,1.0}, {0.0, 0.97, 1.0, 0.03}  /* outer boxes */
				 , {0.85, 0.75, 0.12, 0.03 }
	};
SbRectangle goal0 = {0.94, 0.63, 0.03, 0.12};
////////////////////
//// levels end ////
////////////////////




#endif  // SBPLATFORMER_H

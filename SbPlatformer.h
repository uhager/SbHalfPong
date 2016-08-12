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
const int CONTROLLER_DEADZONE = 7000;
const std::string name = "Platformer";
const double GRAVITY = 3.5e-6;
const double JUMP = 1.0/700.0;
const double FRICTION = 4e-7; 
const double PLAYER_VELOCITY = 1.0/2000.0;
const double STEP_SIZE = LEVEL_WIDTH / 20;
  
class Exit;
class Platform;
class Level;
class Platformer;


/* MovementLimits and MovementRange are used to define moving platforms
 */
struct MovementLimits
{
  MovementLimits(uint32_t l, uint32_t r, uint32_t t, uint32_t b)
  : left(l), right(r), top(t), bottom(b)
  {}
  MovementLimits() = default;

  uint32_t left = 0;
  uint32_t right = 0;
  uint32_t top = 0;
  uint32_t bottom = 0;
};


struct MovementRange
{
  MovementRange(double l, double r, double t, double b)
  : left(l), right(r), top(t), bottom(b)
  {}
  MovementRange() = default;
  
  MovementLimits to_limits(uint32_t width, uint32_t height) {
    MovementLimits result;
    result.left = left * width;
    result.right = right * width;
    result.top = top * height;
    result.bottom = bottom * height;
    return result;
  }

  double left = 0;
  double right = 0;
  double top = 0;
  double bottom = 0;
};


struct Velocity
{
  Velocity(double xdir, double ydir)
  : x(xdir), y(ydir)
  {}
  Velocity() = default;
  
  double x = 0;
  double y = 0;
};



class Player : public SbObject
{
 public:
  Player(const SbDimension* ref);

  bool check_exit(const Exit& goal);
  void handle_event(const SDL_Event& event);
  int move(const std::vector<std::unique_ptr<SbObject>>& level);
  void follow_platform();
  //  void render();
  /*! Reset after goal.
   */
  void reset();

 private:
  bool check_air_deltav( double sensitivity );

  const std::unique_ptr<SbObject>* standing_on_ = nullptr;
  bool exit_ = false;
  double velocity_max_ = PLAYER_VELOCITY;
  double velocity_jump_ = JUMP;
  bool on_surface_ = true;
  /*! allowed_air_deltav_ is the number of direction changes allowed in mid-air. in_air_deltav_ keeps track of direction changes in mid-air
   */
  uint32_t in_air_deltav_ = 0;
  uint32_t allowed_air_deltav_ = 2;
  double controller_sensitivity_ = 0.1;
  double friction_ = FRICTION;
  double step_size = STEP_SIZE;
  double movement_start_position;
  
};



class Platform : public SbObject
{
  friend class Level;
 public:
  Platform(int x, int y, int width, int height, const SbDimension* ref);
  Platform( SbRectangle bounding_box, const SbDimension* ref );
    int move();
    
 private:
    Velocity velocity_;
    MovementLimits limits_;
    void set_velocities(double x, double y);
    void set_velocities(Velocity v);
    void set_limits(MovementLimits limit);
};


class Exit : public SbObject
{
 public:
  Exit(int x, int y, int width, int height);
  Exit(SbRectangle box, const SbDimension* ref);
};


class Level
{
 public:
  Level(uint32_t num, const SbDimension* window_ref);

  void create_level(uint32_t num);
   Exit const& exit() const {return *exit_;}
  std::vector<std::unique_ptr<SbObject>> const& platforms() const {return platforms_; }
  uint32_t width() { return dimension_.w; }
  uint32_t height() {return dimension_.h; }
  void render(const SDL_Rect &camera);
  uint32_t level_number() { return level_num_; }
  //  void handle_event(const SDL_Event& event);
  void move();
  void update_size();
  const SbDimension* get_dimension() const {return &dimension_;} 
  
 private:
  SbDimension dimension_ = {100,100};
  const SbDimension* window_ref_;
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
LevelCoordinates(SbDimension d, std::vector<SbRectangle> t, SbRectangle g, std::vector<MovementRange> r, std::vector<Velocity> v)
: dimension(d), tiles(t),goal(g), ranges(r), velocities(v)
  {}
  SbDimension dimension;
  std::vector<SbRectangle> tiles;
  SbRectangle goal;
  std::vector<MovementRange> ranges;
  std::vector<Velocity> velocities;
};


////////////////////
////// levels //////
////////////////////
std::vector<LevelCoordinates> levels;

SbDimension dim0 = {LEVEL_WIDTH, LEVEL_HEIGHT};
std::vector<SbRectangle> lev0 = {{0,0,1.0,0.03}, {0.97,0.0,0.03,1.0}, {0.0,0.0,0.03,1.0}, {0.0, 0.97, 1.0, 0.03}  /* outer boxes */
				 , {0.85, 0.75, 0.12, 0.03 }
				 ,{0.03,0.77,0.12,0.03}, {0.18, 0.57, 0.12, 0.03}, {0.03, 0.37, 0.12, 0.03} 
	};
SbRectangle goal0 = {0.03, 0.25, 0.03, 0.12};
std::vector<MovementRange> range0 = { {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} /* outer frame */
				      , {0, 0, 0.15, 0}
				      , {0, 0.08, 0.0, 0.0}, {0.02, 0.1, 0.07, 0.05}, {0, 0, 0, 0}
};
std::vector<Velocity> velocity0 = { {0, 0}, {0, 0}, {0, 0}, {0, 0}
				    ,{0, 0.00005}
				    ,{ 0.00005, 0}, {0.00003, 0.00003}, {0,0}
};


////////////////////
//// levels end ////
////////////////////




#endif  // SBPLATFORMER_H

#include <string>
#include <stdexcept>
#include <memory>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "SbTexture.h"
#include "SbTimer.h"
#include "SbFont.h"

#include "SbPlatformer.h"



/*! Player implementation
 */
Player::Player()
  : SbObject((int)(0.9*LEVEL_WIDTH), (int)(0.9*LEVEL_HEIGHT), (int)(0.02*LEVEL_WIDTH), (int)(0.07*LEVEL_HEIGHT))
{
  velocity_y_ = 0;
  velocity_x_ = 0;
  velocity_ = 1.0/5000.0;
  texture_ = std::make_shared<SbTexture>();
  texture_->from_rectangle(window->renderer(), bounding_rect_.w, bounding_rect_.h, color_ );
  name_ = "player";
}


bool
Player::check_air_deltav(double sensitivity)
{
  bool can_move = false;
  if ( !on_surface_ && in_air_deltav_ > allowed_air_deltav_)
	return can_move;
  if ( !on_surface_ )
    in_air_deltav_ += sensitivity;
  
  return true ;
}


bool
Player::check_exit(const Exit& exit)
{
  exit_ = false;
  SbHitPosition hit = check_hit(exit);
  if ( hit != SbHitPosition::none ) {
    const SDL_Rect& target = exit.bounding_rect();
    bounding_rect_.x =  target.x + ( target.w - bounding_rect_.w ) / 2;  
    bounding_rect_.y =  target.y + ( target.h - bounding_rect_.h ) / 2;
    move_bounding_box();
    exit_ = true;
  }
  return exit_;
}



void
Player::handle_event(const SDL_Event& event)
{
  double sensitivity = 1.0 ; // controller needs slower acceleration
  SbControlDir direction = SbControlDir::none;

  if( event.type == SDL_KEYDOWN && event.key.repeat == 0  ) {
    switch( event.key.keysym.sym  ) {
    case SDLK_UP: case SDLK_SPACE: direction = SbControlDir::up; break;
    case SDLK_DOWN: direction = SbControlDir::down; break;
    case SDLK_LEFT: direction = SbControlDir::left; break;
    case SDLK_RIGHT: direction = SbControlDir::right; break;
    }
  }
  else if( event.type == SDL_CONTROLLERAXISMOTION &&  event.jaxis.which == 0 ) {
      sensitivity = controller_sensitivity_;
    switch ( event.jaxis.axis ) {
    case 0: //X axis motion
      if ( event.jaxis.value < -CONTROLLER_DEADZONE )
	direction = SbControlDir::left;
      else if ( event.jaxis.value > CONTROLLER_DEADZONE )
	direction = SbControlDir::right;
      break;
    case 1:
      if ( event.jaxis.value < -CONTROLLER_DEADZONE )
	direction = SbControlDir::up;
      else if ( event.jaxis.value > CONTROLLER_DEADZONE )
	direction = SbControlDir::down;
      break;
    }
  }
  else if( event.type == SDL_JOYAXISMOTION &&  event.jaxis.which == 0 ) {
    switch ( event.jaxis.axis ) {
    case 0: //X axis motion
      sensitivity = controller_sensitivity_;
      if ( event.jaxis.value < -CONTROLLER_DEADZONE )
	direction = SbControlDir::left;
      else if ( event.jaxis.value > CONTROLLER_DEADZONE )
	direction = SbControlDir::right;
      break;
    // case 1:
    //   sensitivity = 0.1;
    //   if ( event.jaxis.value < -CONTROLLER_DEADZONE )
    // 	direction = SbControlDir::up;
    //   else if ( event.jaxis.value > CONTROLLER_DEADZONE )
    // 	direction = SbControlDir::down;
    //   break;
    }
  }
  else if (   event.type == SDL_CONTROLLERBUTTONDOWN
	      && event.cbutton.which == 0
	      && event.cbutton.button == SDL_CONTROLLER_BUTTON_A ) {
	direction = SbControlDir::up;   
  }
  else {
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_UP] )
      direction = SbControlDir::up; //velocity_y_ -= velocity_;
    if (state[SDL_SCANCODE_DOWN] )
      direction = SbControlDir::down; //velocity_y_ += velocity_;
    if (state[SDL_SCANCODE_LEFT] )
      direction = SbControlDir::left; //velocity_x_ -= velocity_;
    if (state[SDL_SCANCODE_RIGHT])
      direction = SbControlDir::right; //velocity_x_ += velocity_;
  }

  switch (direction) {
  case SbControlDir::up :
    if (on_surface_) {
      velocity_y_ = -1 * ( velocity_jump_ * sensitivity );
      on_surface_ = false;
      in_air_deltav_ = 0;
    }
    break;
  case SbControlDir::left :
    if (velocity_x_ > -1*velocity_max_ && check_air_deltav(sensitivity) ) {
      velocity_x_ -=  ( velocity_ * sensitivity );
    }
    break;
  case SbControlDir::right :
    if (velocity_x_ < velocity_max_ && check_air_deltav(sensitivity) ) 
      velocity_x_ +=  ( velocity_ * sensitivity );
    break;
  default:
    break;
  }
}



int
Player::move(const std::vector<std::unique_ptr<SbObject>>& level)
{
  int result = 0;
  if ( exit_ ) {
    return result;
  }


  Uint32 deltaT = timer_.get_time();
  if ( !on_surface_ ) {
    velocity_y_ += GRAVITY * deltaT; // gravity
  }
  else if (on_surface_) {
    double fric = friction_ * timer_.get_time();
    if ( velocity_x_ > 0 ) {
      velocity_x_ -= fric;
      if (velocity_x_ < 0) velocity_x_ = 0;
    }
    else {
      velocity_x_ += fric;
      if (velocity_x_ > 0) velocity_x_ = 0;
    }
  }

  int x_step = (int)( window->width() * velocity_x_ * deltaT);
  int y_step = (int)( window->height() * velocity_y_ * deltaT);  
  bounding_rect_.y += y_step;
  bounding_rect_.x += x_step;

  //  int hits = 0 ;   // can only hit max 2 tiles at once
  on_surface_ = false;
  for (auto& tile: level){
    SbHitPosition hit = check_hit(*tile);
    if ( hit == SbHitPosition::none )
      continue;
    else {
      //     ++hits;
      switch (hit) {
      case SbHitPosition::left :
	if (velocity_x_ > 0 )
	  velocity_x_ = 0;
	bounding_rect_.x = tile->pos_x() - bounding_rect_.w;
	break;
      case SbHitPosition::right :
	if (velocity_x_ < 0 )
	  velocity_x_ = 0;  
	bounding_rect_.x = tile->pos_x() + tile->width();
	break;
      case SbHitPosition::top :
	velocity_y_ = tile->velocity_y();
	bounding_rect_.y = tile->pos_y() - bounding_rect_.h;
	on_surface_ = true;
	//in_air_deltav_ = 0;
	break;
      case SbHitPosition::bottom :
	if (velocity_y_ < 0 )
	  velocity_y_ *= -1;  
	break;
      default:
	//	--hits;
	break;
      }
      // if ( hits == 2 )
      // 	break;
    }
  }
  move_bounding_box();
  timer_.start();
  return result;
}



void
Player::reset()
{
  exit_ = false;
  velocity_x_ = 0;
  velocity_y_ = 0;
  bounding_rect_.x = (int)(0.9*LEVEL_WIDTH);
  bounding_rect_.y = (int)(0.9*LEVEL_HEIGHT);
  move_bounding_box();
  timer_.start();
}



/*! Platform implementation
 */
Platform::Platform(int x, int y, int width, int height)
  : SbObject(x, y, width, height)
{
  color_ = {40, 40, 160, 0};
  texture_ = std::make_shared<SbTexture>();
  texture_->from_rectangle( window->renderer(), bounding_rect_.w, bounding_rect_.h, color_ );
  name_ = "tile";
}



int
Platform::move()
{
  Uint32 deltaT = timer_.get_time();
  
  if (velocity_.x > 0 && limits_.left != limits_.right ) {
    if ( bounding_rect_.x + bounding_rect_.w >= limits_.right ) {
      if ( velocity_x_ > 0 ) velocity_x_ *= -1;
    }
    else if( bounding_rect_.x < limits_.left ) {
      if (velocity_x_ < 0 ) velocity_x_ *= -1;
    }
  }
  if (velocity_.y > 0 && limits_.top != limits_.bottom ) {
    if ( bounding_rect_.y + bounding_rect_.h >= limits_.bottom ) {
      if ( velocity_y_ > 0 ) velocity_y_ *= -1;
    }
    else if ( bounding_rect_.y < limits_.top ) {
      if ( velocity_y_ < 0 ) velocity_y_ *= -1;
    }
  }

  bounding_box_.y += velocity_y_ * deltaT;
  bounding_box_.x += velocity_x_ * deltaT;

  move_bounding_rect();
  timer_.start();
  return 0;
}


void
Platform::set_velocities(double x, double y)
{
  velocity_= {x, y};
  velocity_x_ = x;
  velocity_y_ = y;
}


void
Platform::set_velocities(Velocity v)
{
  velocity_= v;
  velocity_x_ = v.x;
  velocity_y_ = v.y;
}


void
Platform::set_limits(MovementLimits limit)
{
  limits_.left = bounding_rect_.x - limit.left;
  limits_.right = bounding_rect_.x + bounding_rect_.w + limit.right;
  limits_.top = bounding_rect_.y - limit.top;
  limits_.bottom = bounding_rect_.y + bounding_rect_.h + limit.bottom;
}



/*! Exit
 */
Exit::Exit(int x, int y, int width, int height)
  : SbObject(x, y, width, height)
{
  color_ = {200, 100, 100};
  texture_ = std::make_shared<SbTexture>();
  texture_->from_rectangle(window->renderer(), bounding_rect_.w, bounding_rect_.h, color_ );
  name_ = "goal";
}




/*! Level implementation
 */
Level::Level(uint32_t num)
  : level_num_(num)
{
  create_level(level_num_);
}


void
Level::create_level(uint32_t num)
{
  if ( !platforms_.empty() )
    platforms_.clear();
  
  if (num > levels.size() )
    throw std::runtime_error("[Level::create_level] No level found for level number = " + std::to_string(num)  );

  std::vector< SbRectangle > &coords = (levels.at(num).tiles);
  SbRectangle& goal = levels.at(num).goal;
  std::vector<MovementRange>& ranges = (levels.at(num).ranges);
  std::vector<Velocity>& vels = (levels.at(num).velocities);
  for ( uint32_t i = 0; i < coords.size(); ++i ){
    int x = coords.at(i).x * width_;
    int y = coords.at(i).y * height_;
    int w = coords.at(i).w * width_;
    int h = coords.at(i).h * height_;
    
    Platform* p = new Platform( x, y, w, h );
    if (ranges.size() > i && vels.size() > i){
      MovementRange& rg = ranges.at(i);
      MovementLimits lmt = rg.to_limits(width_, height_);
      p->set_limits(lmt);
      p->set_velocities(vels.at(i));
    }
    
    platforms_.emplace_back( std::unique_ptr<SbObject>( p ) );
  }
  exit_ = std::unique_ptr<Exit>( new Exit{ (int)(goal.x*LEVEL_WIDTH), (int)(goal.y*LEVEL_HEIGHT), (int)(goal.w*LEVEL_WIDTH), (int)(goal.h*LEVEL_HEIGHT) } );
}


void
Level::move()
{
  for (auto& p: platforms_)
    p->move();
}


void
Level::render(const SDL_Rect &camera)
{
    for (auto& t: platforms_)
      t->render(camera);
    exit_->render( camera );
}



Platformer::Platformer()
{
  SbObject::window = &window_ ;
  SbWorld::window = &window_;
  world_ = {LEVEL_WIDTH, LEVEL_HEIGHT};
  SbObject::world = &world_;

  for (int i = 0; i < SDL_NumJoysticks(); ++i) {
    if (SDL_IsGameController(i)) {
        game_controller_ = SDL_GameControllerOpen(i);
        if (game_controller_) {
            break;
        }
    }
  }

  levels.emplace_back(lev0, goal0, range0, velocity0);

  initialize();
}


Platformer::~Platformer()
{
  SDL_GameControllerClose( game_controller_ );
  game_controller_ = nullptr;
}


void
Platformer::initialize()
{
  camera_ = { 0, 0, window_.width(), window_.height() };

  SbFont font("resources/FreeSans.ttf", 120 );
  // SbFont::handle font = SbFont::handle( TTF_OpenFont( "resources/FreeSans.ttf", 120 ), DeleteFont() );
  // if ( !font )
  //   throw std::runtime_error( "TTF_OpenFont: " + std::string( TTF_GetError() ) );

  player_ = std::unique_ptr<Player>( new Player );
  level_ = std::unique_ptr<Level>( new Level(current_level_) );
  fps_display_ = std::unique_ptr<SbFpsDisplay>( new SbFpsDisplay( font.font() ) );
  
}



void
Platformer::reset()
{
  if ( (++current_level_) == levels.size() ) {
    current_level_ = 0;
  }
  level_->create_level( current_level_ );
  player_->reset();
  reset_timer_.reset();
  in_exit_ = false;
}


Uint32
Platformer::reset_game(Uint32 interval, void *param )
{
  ((Platformer*)param)->reset();
  return(0);
}



void
Platformer::run()
{
    SDL_Event event;
    bool quit = false;

    
    while (!quit) {
      /// begin event polling
      while( SDL_PollEvent( &event ) ) {
	if (event.type == SDL_QUIT) quit = true;
	else if (event.type == SDL_KEYDOWN ) {
	  switch ( event.key.keysym.sym ) {
	  case SDLK_ESCAPE:
	    quit = true;
	    break;
	  }
	}
	else if (   event.type == SDL_CONTROLLERBUTTONDOWN
		    && event.cbutton.which == 0
		    && event.cbutton.button == SDL_CONTROLLER_BUTTON_B ) {
	  quit = true;
	}
	window_.handle_event(event);
	world_.handle_event( event );
	player_->handle_event(event);
	//	level_->handle_event( event );
	fps_display_->handle_event(event);
      }
      /// end event polling

      	if ( reset_timer_.get_time() > 1500 )
	  reset();
	

      level_->move();
      player_->move(level_->platforms());
      player_->center_camera(camera_, LEVEL_WIDTH, LEVEL_HEIGHT);
      if ( !in_exit_ ) {
	in_exit_ = player_->check_exit(level_->exit());
	if (in_exit_) {
	  //	  SDL_AddTimer(2000, Maze::reset_game, this);
	  reset_timer_.start();
	}
      }
      fps_display_->update();
      
      SDL_RenderClear( window_.renderer() );
      level_->render( camera_ );
      fps_display_->render();
      player_->render( camera_ );
      SDL_RenderPresent( window_.renderer() );

    }
}


int main()
{
  sdl_init();
  try {
    Platformer plat;
    plat.run();
  }
  catch (const std::exception& expt) {
    std::cerr << expt.what() << std::endl;
  }
  sdl_quit();
  return 0;
}
  


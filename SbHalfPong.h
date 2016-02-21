/* SbHalfPong.h
author: Ulrike Hager
*/

#ifndef SBHALFPONG_H
#define SBHALFPONG_H


#include <random>
#include <string>


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "SbObject.h"
#include "SbMessage.h"


class Ball;
class Spark;
class Paddle;

class Paddle : public SbObject
{
public:
  Paddle();
  void handle_event(const SDL_Event& event);
  int move();
};



class Ball : public SbObject
{
public:
  Ball();
  //  void handle_event(const SDL_Event& event);
  /*! \retval 1 if ball in goal
    \retval 0 else
   */
  int move(const SDL_Rect& paddleBox);
  void render();
  /*! Reset after goal.
   */
  void reset();
  static Uint32 resetball(Uint32 interval, void *param );
  Uint32 remove_spark(Uint32 interval, void *param, int index );

  
private:
  int goal_ = 0;
  std::vector<Spark> sparks_;
  std::default_random_engine generator_;
  std::uniform_int_distribution<int> distr_number { 15, 30 };
  std::normal_distribution<double> distr_position { 0.0, 0.01 };
  std::normal_distribution<double> distr_size { 0.003, 0.002 };
  std::uniform_int_distribution<int> distr_lifetime { 100, 400 };

  void create_sparks();
  void center_in_front(const SDL_Rect& paddleBox);
  void delete_spark(int index);
};



class Spark : public SbObject
{
  friend class Ball;
public:
  Spark(double x, double y, double width, double height);
  ~Spark();

  static Uint32 expire(Uint32 interval, void* param);

  void set_texture(SbTexture* tex) {texture_ = tex;}
  int index() { return index_;}
  bool is_dead() {return is_dead_;}  
  Uint32 lifetime() { return lifetime_;}

private:
  //  SDL_TimerID spark_timer_;
  int index_ = 0;
  bool is_dead_ = false;
  Uint32 lifetime_ = 100;
};


  

class GameOver : public SbMessage
{
 public:
  GameOver(TTF_Font *font);
};



class HighScore : public SbMessage
{
 public:
  HighScore(TTF_Font *font, std::string filename = "halfpong.save");
  void new_highscore( int score );
  void old_highscore( int score );
  int read_highscore();
  void write_highscore();

 private:
  int highscore_;
  std::string savefile_;
};

#endif  // SBHALFPONG_H

/*! \file SbMessage.h
  part of SDL2-basic
  author: Ulrike Hager
 */

#ifndef SBMESSAGE_H
#define SBMESSAGE_H

#include <deque>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "SbObject.h"


class SbMessage : public SbObject
{
public:
  SbMessage(SbRectangle box, SbDimension& ref);

  void set_font(std::shared_ptr<TTF_Font> font){ font_ = font;}
  void set_text(std::string text);

 protected: 
  std::shared_ptr<TTF_Font> font_ = nullptr;
};



class SbFpsDisplay : public SbMessage
{
 public:
  //  SbFpsDisplay(std::shared_ptr<TTF_Font> font, double x = 0, double y = 0, double width = 0.06, double height= 0.035);
  SbFpsDisplay(std::shared_ptr<TTF_Font> font, SbRectangle box, SbDimension& ref);
  void handle_event(const SDL_Event& event) override;
  void set_number_frames( uint32_t n );
  void update();
  
 private:
  uint32_t n_frames_ = 250;
  double sum_ = 0;
  std::deque<double> times_;
  
};


class SbHighScore : public SbMessage
{
 public:
  //  SbHighScore(std::shared_ptr<TTF_Font> font, std::string filename = "game.save", std::string prefix = "Your result", std::string postfix = "" );
  SbHighScore(std::shared_ptr<TTF_Font> font, SbRectangle box, SbDimension& ref);
  bool check_highscore( uint32_t score, bool(SbHighScore::*fctn)(uint32_t, uint32_t), uint32_t level = 0, double multiplier = 1.0);
  std::vector<uint32_t> highscores() { return highscores_; }
  std::vector<uint32_t> read_highscores( );
  void write_highscores( );

  bool higher(uint32_t score, uint32_t level);
  bool lower(uint32_t score, uint32_t level);

  void set_precision(uint32_t p) {precision_ = p;}

  std::string savefile = "game.save", prefix = "Your result", postfix = "";

 private:
  std::vector<uint32_t> highscores_;
  uint32_t precision_ = 1;
};



#endif  //  SBMESSAGE_H


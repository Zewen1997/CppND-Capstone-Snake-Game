#ifndef GAME_H
#define GAME_H

#include <random>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "obstacle.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  void SetObstacle(std::unique_ptr<Obstacle> obstacle);
  void WaitForRestart();
  void Reset();
  void PlaceBonusFood();
  void BonusFoodTimer();

 private:
  Snake snake;
  SDL_Point food;
  SDL_Point bonus_food;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  std::unique_ptr<Obstacle> obstacle;

  std::thread bonusFoodThread;
  std::mutex mutex;
  std::condition_variable condition_var;

  int score{0};

  void PlaceFood();
  void Update(Renderer &renderer);

  bool is_bonus_food_active{false};
};

#endif
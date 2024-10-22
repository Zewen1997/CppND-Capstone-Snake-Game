#include "game.h"
#include <iostream>
#include "SDL.h"


Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  PlaceFood();
}

void Game::SetObstacle(std::unique_ptr<Obstacle> obstacle){
  this->obstacle = std::move(obstacle);
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  // Create a thread to handle the snake's movement
  /* std::thread movement_thread([this, &controller, &renderer, &running]() {
  while (running) {
      
      {
        std::lock_guard<std::mutex> lock(mtx); 
        controller.HandleInput(running, snake);
        Update(renderer); 
      } 
      std::this_thread::sleep_for(std::chrono::milliseconds(50));  
    }
  }); */

  while (running) {
    frame_start = SDL_GetTicks();

    controller.HandleInput(running, snake);
    Update(renderer);
    renderer.Render(snake, food, bonus_food, *obstacle, is_bonus_food_active);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
  
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::Update(Renderer &renderer) {
  if (!is_bonus_food_active && (rand() % 10000) < 10){ // Assuming a 0.1% chance of generating bonus food for every frame (not every second!)
    PlaceBonusFood(); 
    is_bonus_food_active = true;
    bonusFoodThread = std::thread(&Game::BonusFoodTimer, this); 
    bonusFoodThread.detach();  
  }


  if (obstacle && obstacle->checkCollision(snake)) {
    snake.alive = false; 
  }

  if (!snake.alive) 
  {
  renderer.Render(snake, food, bonus_food, *obstacle, is_bonus_food_active); // Force the snake's death state to be rendered once, with the snake's head in red (otherwise the snake's head is shown in blue)
  WaitForRestart();
  Reset();
  return;
  }

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }

  // Check if the snake ate bonus food
  if (is_bonus_food_active && bonus_food.x == static_cast<int>(snake.head_x) && bonus_food.y == static_cast<int>(snake.head_y)) {
    score += 5; 
    is_bonus_food_active = false; 
  }
}


void Game::WaitForRestart(){
  SDL_Event e;
  bool wait = true;
  while (wait) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        exit(0);  
      } else if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) {
          wait = false;
        }
      }
    }
  }
}

void Game::Reset(){
  snake = Snake(32, 32);
  PlaceFood();  
  obstacle = std::make_unique<Obstacle>(640, 640, 32, 32, 8);
  score = 0;  
}

void Game::PlaceBonusFood(){
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    if (!snake.SnakeCell(x, y)) {
      bonus_food.x = x;
      bonus_food.y = y;
      return;
    }
  }
}


void Game::BonusFoodTimer()
{
  const int bonusSeconds = 8;
  auto startTime = std::chrono::high_resolution_clock::now();
  std::unique_lock<std::mutex> lock(mutex);
  while (is_bonus_food_active)
  {
    lock.unlock();
    auto current_Time = std::chrono::high_resolution_clock::now();
    auto elapsed_Seconds = std::chrono::duration_cast<std::chrono::seconds>(current_Time - startTime).count();
    if (elapsed_Seconds >= bonusSeconds)
    {
      // Bonus food time is up
      is_bonus_food_active = false;
      bonus_food.x = -1;
      bonus_food.y = -1;
      break;
    }
    lock.lock();
    // Wait for a short interval or until the condition_variable is notified
    condition_var.wait_for(lock, std::chrono::milliseconds(800));
  }}


int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }
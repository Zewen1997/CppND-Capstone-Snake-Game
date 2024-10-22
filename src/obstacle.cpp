#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include "SDL.h"
#include "obstacle.h"

Obstacle::Obstacle(std::size_t screenWidth, std::size_t screenHeight, std::size_t gridWidth, std::size_t gridHeight, int numRects): 
    screen_width(screenWidth), screen_height(screenHeight), grid_width(gridWidth), grid_height(gridHeight) {
    // Generate connected rectangles
    generateConnectedRects(screenWidth, screenHeight, gridWidth, gridHeight, numRects);
}

void Obstacle::generateConnectedRects(std::size_t screenWidth, std::size_t screenHeight, std::size_t gridWidth, std::size_t gridHeight, int numRects) {
    // First, generate the first rectangle
    SDL_Rect rect;
    rect.w = screenWidth / gridWidth;
    rect.h = screenHeight / gridHeight;
    rect.x = (rand() % gridWidth) * rect.w;
    rect.y = (rand() % gridHeight) * rect.h;

    // Add the first rectangle to the collection
    rects.push_back(rect);

    // Generate the subsequent connected rectangles
    for (int i = 1; i < numRects; ++i) {
        // Get the position of the last rectangle
        SDL_Rect prevRect = rects.back();
        SDL_Rect newRect = prevRect;

        // Randomly choose a direction to generate the adjacent rectangle
        int direction = rand() % 4;
        switch (direction) {
            case 0: // Move up
                newRect.y -= newRect.h;
                break;
            case 1: // Move down
                newRect.y += newRect.h;
                break;
            case 2: // Move left
                newRect.x -= newRect.w;
                break;
            case 3: // Move right
                newRect.x += newRect.w;
                break;
        }

        // Ensure the new rectangle does not go beyond the screen boundaries
        if (newRect.x >= 0 && newRect.x < screenWidth &&
            newRect.y >= 0 && newRect.y < screenHeight) {
            rects.push_back(newRect);
        } else {
            // If it exceeds the boundary, regenerate this rectangle
            --i;
        }
    }
}

bool Obstacle::checkCollision(Snake const &snake) const {
  
  SDL_Rect head_rect;
  head_rect.w = screen_width / grid_width;
  head_rect.h = screen_height / grid_height;
  head_rect.x = static_cast<int>(snake.head_x) * head_rect.w;
  head_rect.y = static_cast<int>(snake.head_y) * head_rect.h;

  
  for (const SDL_Rect &rect : rects) {
    if (SDL_HasIntersection(&head_rect, &rect)) {
      return true;  
    }
  }
  return false;
}

const std::vector<SDL_Rect>& Obstacle::getRects() const {
    return rects;
}
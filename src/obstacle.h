#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <cmath>
#include <iostream>
#include <random>
#include <vector>
#include "SDL.h"
#include "snake.h"

class Obstacle {
    private:
        std::vector<SDL_Rect> rects;
        int numRects;
        std::size_t screen_width;
        std::size_t screen_height;
        std::size_t grid_width;
        std::size_t grid_height;

    public:
    // Constructor
    Obstacle(std::size_t screenWidth, std::size_t screenHeight, std::size_t gridWidth, std::size_t gridHeight, int numRects);

    // Move constructor
    Obstacle(Obstacle&& other){
        rects = std::move(other.rects);
        numRects = other.numRects;
    }

    // Move assignment operator
    Obstacle& operator=(Obstacle&& other){
        if (this != &other) {
            rects = std::move(other.rects);
            numRects = other.numRects;
            // Any other resource that needs to be moved
        }
        return *this;
    }

    // Generate connected rectangles
    void generateConnectedRects(std::size_t screenWidth, std::size_t screenHeight, std::size_t gridWidth, std::size_t gridHeight, int numRects);
    bool checkCollision(Snake const &snake) const;
    const std::vector<SDL_Rect>& getRects() const;
};

#endif
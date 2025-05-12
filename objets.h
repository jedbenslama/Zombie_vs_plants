#ifndef OBJETS_H
#define OBJETS_H

#include <SDL.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRAVITY 0.5
#define JUMP_VELOCITY -10
#define PLAYER_SPEED 5

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

SDL_Rect ground = { 0, SCREEN_HEIGHT - 100, 2000, 100 };

#endif

bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

SDL_Rect ground = { 0, SCREEN_HEIGHT - 100, 2000, 100 };

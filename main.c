#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRAVITY 0.5
#define JUMP_VELOCITY -10
#define PLAYER_SPEED 5

#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include "objets.h"


typedef struct {
    SDL_Rect rect;
    float velY;
    bool onGround;
} Player;

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    SDL_Window* window = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* background = IMG_LoadTexture(renderer, "assets/background.jpg");
    SDL_Texture* brick = IMG_LoadTexture(renderer, "assets/brick.jpg");
    SDL_Texture* soltexture = IMG_LoadTexture(renderer, "assets/soltexture.jpg");
    SDL_Texture* character = IMG_LoadTexture(renderer, "assets/personnage_d.png");
    
    // x y largeur hauteur

    SDL_Rect platform = { 500, 400, 200, 50 };
    SDL_Rect platform2 = { 200, 400, 80, 300 };

    Player player = { .rect = {100, SCREEN_HEIGHT - 200, 50, 50}, .velY = 0, .onGround = false };

    float cameraX = 0;

    bool running = true;
    SDL_Event event;

    while (running) {
        if(player.rect.y>2000){
            exit(123);
        }
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_LEFT]){ player.rect.x -= PLAYER_SPEED; character = IMG_LoadTexture(renderer, "assets/personnage_g.png");}
        if (keystate[SDL_SCANCODE_RIGHT]){ player.rect.x += PLAYER_SPEED; character = IMG_LoadTexture(renderer, "assets/personnage_d.png");}
        if ((keystate[SDL_SCANCODE_SPACE] || keystate[SDL_SCANCODE_UP]) && player.onGround) {
            player.velY = JUMP_VELOCITY;
            player.onGround = false;
        }
        if (keystate[SDL_SCANCODE_G]){ platform.x+=10;}
        if (keystate[SDL_SCANCODE_F]){ platform.x-=10;}
        if (keystate[SDL_SCANCODE_R]){ platform.y-=10;}
        if (keystate[SDL_SCANCODE_T]){ platform.y+=10;}

        player.rect.y += player.velY;
        player.velY += GRAVITY;

        if (checkCollision(player.rect, ground)) {
            player.rect.y = ground.y - player.rect.h;
            player.velY = 0;
            player.onGround = true;
        }
        else if (checkCollision(player.rect, platform) && player.velY >= 0 && player.rect.y <= platform.y + 20) {
            player.rect.y = platform.y - player.rect.h;
            player.velY = 0;
            player.onGround = true;
        }else if (checkCollision(player.rect, platform2) && player.velY >= 0 && player.rect.y <= platform2.y + 20) {
            player.rect.y = platform2.y - player.rect.h;
            player.velY = 0;
            player.onGround = true;
        } else {
            player.onGround = false;
        }

        cameraX = player.rect.x + player.rect.w / 2 - SCREEN_WIDTH / 2;
        if (cameraX < 0) cameraX = 0;

        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, background, NULL, NULL);

        SDL_Rect groundQuandBouge = ground;
        groundQuandBouge.x -= (int)cameraX;
        SDL_RenderCopy(renderer, soltexture, NULL, &groundQuandBouge);

        SDL_Rect platformQuandBouge = platform;
        platformQuandBouge.x -= (int)cameraX;
        SDL_RenderCopy(renderer, brick, NULL, &platformQuandBouge);

        SDL_Rect platform2QuandBouge = platform2;
        platform2QuandBouge.x -= (int)cameraX;
        SDL_RenderCopy(renderer, brick, NULL, &platform2QuandBouge);

        SDL_Rect playerScreenCentre = player.rect;
        playerScreenCentre.x -= (int)cameraX;
        SDL_RenderCopy(renderer, character, NULL, &playerScreenCentre);

        SDL_RenderPresent(renderer);
        SDL_Delay(10); // 100 FPS
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(brick);
    SDL_DestroyTexture(soltexture);
    SDL_DestroyTexture(character);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}

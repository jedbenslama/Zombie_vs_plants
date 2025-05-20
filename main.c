#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include <stdlib.h>
#include "objets.h"

typedef struct {
    SDL_Rect rect;
    SDL_Texture* texture;
} Button;

typedef struct {
    Button* array;
    int count;
    int capacity;
} ButtonList;

typedef struct {
    SDL_Rect* array;
    int count;
    int capacity;
} PlatformList;

typedef struct {
    SDL_Rect rect;
    float velY;
    bool onGround;
} Player;

void initButtonList(ButtonList* list) {
    list->count = 0;
    list->capacity = 2;
    list->array = malloc(sizeof(Button) * list->capacity);
}

void addButton(ButtonList* list, int x, int y, int w, int h, SDL_Texture* texture) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->array = realloc(list->array, sizeof(Button) * list->capacity);
    }
    list->array[list->count++] = (Button){ .rect = { x, y, w, h }, .texture = texture };
}

void renderButtons(SDL_Renderer* renderer, ButtonList* list, float cameraX) {
    for (int i = 0; i < list->count; i++) {
        SDL_Rect dst = list->array[i].rect;
        dst.x -= (int)cameraX;
        SDL_RenderCopy(renderer, list->array[i].texture, NULL, &dst);
    }
}

void freeButtonList(ButtonList* list) {
    free(list->array);
    list->array = NULL;
    list->count = 0;
    list->capacity = 0;
}

void initPlatformList(PlatformList* list) {
    list->count = 0;
    list->capacity = 4;
    list->array = malloc(sizeof(SDL_Rect) * list->capacity);
}

void addPlatform(PlatformList* list, int x, int y, int w, int h) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->array = realloc(list->array, sizeof(SDL_Rect) * list->capacity);
    }
    list->array[list->count++] = (SDL_Rect){ x, y, w, h };
}

void renderPlatforms(SDL_Renderer* renderer, SDL_Texture* texture, PlatformList* list, float cameraX) {
    for (int i = 0; i < list->count; i++) {
        SDL_Rect dst = list->array[i];
        dst.x -= (int)cameraX;
        SDL_RenderCopy(renderer, texture, NULL, &dst);
    }
}

void freePlatformList(PlatformList* list) {
    free(list->array);
    list->array = NULL;
    list->count = 0;
    list->capacity = 0;
}

void startGame(int level) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    SDL_Window* window = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* background = IMG_LoadTexture(renderer, "assets/background.jpg");
    SDL_Texture* brick = IMG_LoadTexture(renderer, "assets/brick.jpg");
    SDL_Texture* soltexture = IMG_LoadTexture(renderer, "assets/soltexture.jpg");
    SDL_Texture* character = IMG_LoadTexture(renderer, "assets/personnage_d.png");

    SDL_Texture* startButton = IMG_LoadTexture(renderer, "assets/startButton.png");
    SDL_Texture* exitButton = IMG_LoadTexture(renderer, "assets/exitButton.png");

    PlatformList platforms;
    initPlatformList(&platforms);
    ButtonList buttons;
    initButtonList(&buttons);
    
    switch (level) {
        case 0:
            addPlatform(&platforms, 500, 400, 200, 50);
            addButton(&buttons, 700, SCREEN_HEIGHT - 200, 80, 50, startButton);
            addButton(&buttons, 400, SCREEN_HEIGHT - 200, 80, 50, exitButton);
            break;
        case 1:
            addPlatform(&platforms, 500, 400, 200, 50);
            addPlatform(&platforms, 200, 400, 80, 300);
            addPlatform(&platforms, 300, 300, 100, 100);
            break;
        default:
            printf("level non existant");
            return;
    }

    Player player = { .rect = {100, SCREEN_HEIGHT - 200, 50, 50}, .velY = 0, .onGround = false };
    float cameraX = 0;
    SDL_Event event;
    bool running = true;

    while (running) {
        if (player.rect.y > 2000) exit(123);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_LEFT]) { player.rect.x -= PLAYER_SPEED; character = IMG_LoadTexture(renderer, "assets/personnage_g.png"); }
        if (keystate[SDL_SCANCODE_RIGHT]) { player.rect.x += PLAYER_SPEED; character = IMG_LoadTexture(renderer, "assets/personnage_d.png"); }
        if ((keystate[SDL_SCANCODE_SPACE] || keystate[SDL_SCANCODE_UP]) && player.onGround) {
            player.velY = JUMP_VELOCITY;
            player.onGround = false;
        }

        player.rect.y += player.velY;
        player.velY += GRAVITY;

        if (checkCollision(player.rect, ground)) {
            player.rect.y = ground.y - player.rect.h;
            player.velY = 0;
            player.onGround = true;
        } else {
            player.onGround = false;
            for (int i = 0; i < platforms.count; i++) {
                SDL_Rect plat = platforms.array[i];
                if (checkCollision(player.rect, plat) && player.velY >= 0 && player.rect.y <= plat.y - 30) {
                    player.rect.y = plat.y - player.rect.h;
                    player.velY = 0;
                    player.onGround = true;
                    break;
                }
            }
        }

        for (int i = 0; i < buttons.count; i++) {
            if (checkCollision(player.rect, buttons.array[i].rect)) {
                if (i != -1) {
                    freePlatformList(&platforms);
                    freeButtonList(&buttons);
                    SDL_DestroyTexture(background);
                    SDL_DestroyTexture(brick);
                    SDL_DestroyTexture(soltexture);
                    SDL_DestroyTexture(character);
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    IMG_Quit();
                    SDL_Quit();
                    startGame(i);
                    return;
                } else {
                    exit(0);
                }
            }
        }

        cameraX = player.rect.x + player.rect.w / 2 - SCREEN_WIDTH / 2;
        if (cameraX < 0) cameraX = 0;

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, NULL, NULL);

        SDL_Rect groundMoved = ground;
        groundMoved.x -= (int)cameraX;
        SDL_RenderCopy(renderer, soltexture, NULL, &groundMoved);

        renderPlatforms(renderer, brick, &platforms, cameraX);
        renderButtons(renderer, &buttons, cameraX);

        SDL_Rect playerScreen = player.rect;
        playerScreen.x -= (int)cameraX;
        SDL_RenderCopy(renderer, character, NULL, &playerScreen);

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    freePlatformList(&platforms);
    freeButtonList(&buttons);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(brick);
    SDL_DestroyTexture(soltexture);
    SDL_DestroyTexture(character);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

int main() {
    startGame(0);
}

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


typedef struct {
    SDL_Rect rect;
    SDL_Texture* texture;
    int shootTimer;
} Plant;

typedef struct {
    SDL_Rect rect;
    float velX;
    float velY;
    SDL_Texture* texture;
    bool active;
} Projectile;


void updatePlant(Plant* plant, Projectile* projectiles, SDL_Texture* projTexture, Player* player) {
    plant->shootTimer++;
    if (plant->shootTimer >= 120) {
        for (int i = 0; i < 10; i++) {
            if (!projectiles[i].active) {
                projectiles[i].active = true;
                projectiles[i].rect.x = plant->rect.x;
                projectiles[i].rect.y = plant->rect.y;
                projectiles[i].rect.w = 20;
                projectiles[i].rect.h = 20;
                float dx = player->rect.x - plant->rect.x;
                float dy = player->rect.y - plant->rect.y;
                float magnitude = SDL_sqrtf(dx * dx + dy * dy);
                projectiles[i].velX = (dx / magnitude) * 5;
                projectiles[i].velY = (dy / magnitude) * 5;
                projectiles[i].texture = projTexture;
                break;
            }
        }
        plant->shootTimer = 0;
    }
}

void updateProjectiles(Projectile* projectiles) {
    for (int i = 0; i < 10; i++) {
        if (projectiles[i].active) {
            projectiles[i].rect.x += (int)projectiles[i].velX;
            projectiles[i].rect.y += (int)projectiles[i].velY;
            if (projectiles[i].rect.x < 0 || projectiles[i].rect.x > SCREEN_WIDTH + 200) {
                projectiles[i].active = false;
            }
        }
    }
}

void renderProjectiles(SDL_Renderer* renderer, Projectile* projectiles, float cameraX) {
    for (int i = 0; i < 10; i++) {
        if (projectiles[i].active) {
            SDL_Rect dst = projectiles[i].rect;
            dst.x -= (int)cameraX;
            SDL_RenderCopy(renderer, projectiles[i].texture, NULL, &dst);
        }
    }
}

void startGame(int level) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    SDL_Window* window = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* brick = IMG_LoadTexture(renderer, "assets/brick.png");
    SDL_Texture* soltexture = IMG_LoadTexture(renderer, "assets/soltexture.jpg");
    SDL_Texture* character = IMG_LoadTexture(renderer, "assets/droite1.png");

    SDL_Texture* startButton = IMG_LoadTexture(renderer, "assets/startButton.png");
    SDL_Texture* nextButton = IMG_LoadTexture(renderer, "assets/nextButton.png");
    SDL_Texture* backButton = IMG_LoadTexture(renderer, "assets/backButton.png");
    SDL_Texture* exitButton = IMG_LoadTexture(renderer, "assets/exitButton.png");

    SDL_Texture* plantTexture = IMG_LoadTexture(renderer, "assets/plant.png");
    SDL_Texture* projectileTexture = IMG_LoadTexture(renderer, "assets/ball.png");

    PlatformList platforms;
    initPlatformList(&platforms);
    ButtonList buttons;
    initButtonList(&buttons);

    Plant plant;
    
    switch (level) {
        case 0:
            addPlatform(&platforms, 300, 650, 100, 50);
            addPlatform(&platforms, 500, 550, 100, 50);
            addPlatform(&platforms, 700, 450, 100, 50);
            addPlatform(&platforms, 900, 350, 100, 50);
            addButton(&buttons, 100, SCREEN_HEIGHT - 300, 80, 50, exitButton);
            addButton(&buttons, 950, SCREEN_HEIGHT - 650, 80, 50, startButton);
            break;
        case 1:
            addButton(&buttons, 100, SCREEN_HEIGHT - 300, 80, 50, backButton);
            addPlatform(&platforms, 200, 650, 80, 50);
            addPlatform(&platforms, 400, 600, 80, 50);
            addPlatform(&platforms, 600, 600, 80, 50);
            addPlatform(&platforms, 800, 600, 80, 50);
            addPlatform(&platforms, 1000, 600, 80, 50);
            addPlatform(&platforms, 1200, 600, 80, 50);
            addPlatform(&platforms, 1400, 600, 80, 50);

            addPlatform(&platforms, 600, 430, 80, 50);
            addPlatform(&platforms, 800, 430, 80, 50);
            addPlatform(&platforms, 1000, 430, 80, 50);
            addPlatform(&platforms, 1200, 430, 80, 50);
            addPlatform(&platforms, 1400, 430, 80, 50);
            addPlatform(&platforms, 1600, 500, 80, 50);

            addPlatform(&platforms, 400, 330, 80, 50);
            addPlatform(&platforms, 400, 230, 80, 50);

            addPlatform(&platforms, 600, 130, 80, 50);
            addPlatform(&platforms, 800, 130, 80, 50);
            addPlatform(&platforms, 1000, 130, 80, 50);
            addPlatform(&platforms, 1200, 130, 80, 50);
            addPlatform(&platforms, 1400, 130, 80, 50);
            addPlatform(&platforms, 1600, 130, 80, 50);

            addPlatform(&platforms, 1600, 500, 80, 50);

            addButton(&buttons, 2100, 500, 80, 50, nextButton);
            break;
        case 2:
            addPlatform(&platforms, 300, 650, 100, 50);
            addPlatform(&platforms, 500, 550, 100, 50);
            addPlatform(&platforms, 700, 450, 100, 50);
            addPlatform(&platforms, 900, 350, 100, 50);
            addButton(&buttons, 100, SCREEN_HEIGHT - 300, 80, 50, backButton);
            addButton(&buttons, 1800, 320, 80, 50, nextButton);
            break;
        case 3:
            plant.rect = (SDL_Rect){550, 500, 50, 50};
            plant.texture = plantTexture;
            plant.shootTimer = 0;
            addPlatform(&platforms, 300, 650, 100, 50);
            addPlatform(&platforms, 500, 550, 300, 50);
            addPlatform(&platforms, 900, 450, 100, 50);
            addPlatform(&platforms, 1100, 350, 100, 50);
            addButton(&buttons, 100, SCREEN_HEIGHT - 300, 80, 50, backButton);
            addButton(&buttons, 1800, 320, 80, 50, nextButton);
            break;
        default:
            printf("level non existant\n");
            return startGame(0);
            exit(0);
    }

    Player player = { .rect = {100, SCREEN_HEIGHT - 200, 25, 50}, .velY = 0, .onGround = false };
    
    Projectile projectiles[10] = {0};
    float cameraX = 0;
    SDL_Event event;
    bool running = true;

    bool gauchePlatform = false;
    int loop1000 = 0;
    int loop10 = 0;
    int currbg = 0;
    char pathimg[100];
    char num[10];
    int currperso = 0;
    bool droite = true;

    strcpy(pathimg, "assets/bg1.png");
    SDL_Texture* background = IMG_LoadTexture(renderer, pathimg);

    SDL_Rect plantScreen;

    while (running) {
        loop1000+=1;
        if(loop1000==1000){
            loop1000=0;
        }
        loop10+=1;
        if(loop10==10){
            currbg+=1;
            if(currbg>8){
                currbg=1;
            }
            currperso+=1;
            if(currperso>4){
                currperso=1;
            }
            SDL_itoa(currbg, num, 10);
            strcpy(pathimg, "assets/bg");
            strcat(pathimg, num);
            strcat(pathimg, ".png");
            background = IMG_LoadTexture(renderer, pathimg);

            if(player.onGround==false){
                SDL_itoa(1, num, 10);
            }else{
                SDL_itoa(currperso, num, 10);
            }
            
            if(droite){
                strcpy(pathimg, "assets/droite");
                strcat(pathimg, num);
                strcat(pathimg, ".png");
                character = IMG_LoadTexture(renderer, pathimg);
            }
            else{
                strcpy(pathimg, "assets/gauche");
                strcat(pathimg, num);
                strcat(pathimg, ".png");
                character = IMG_LoadTexture(renderer, pathimg);
            }

            loop10=0;
        }
        if (player.rect.y > 2000){
            return startGame(level);
            exit(0);
        };



        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_LEFT]){
            player.rect.x -= PLAYER_SPEED; droite=false;
        }
        if (keystate[SDL_SCANCODE_RIGHT]){
            player.rect.x += PLAYER_SPEED; droite=true;
        }
        if ((keystate[SDL_SCANCODE_SPACE] || keystate[SDL_SCANCODE_UP]) && player.onGround) {
            player.velY = JUMP_VELOCITY;
            player.onGround = false;
        }

        player.rect.y += player.velY;
        player.velY += GRAVITY;

        if (keystate[SDL_SCANCODE_J]){
            player.rect.x=platforms.array[3].x+30;
            player.rect.y=platforms.array[3].y-30;
        }

        switch (level) {
            case 2:
                if(platforms.array[3].x<1600 && gauchePlatform==true){
                    platforms.array[3].x+=4;
                }else{
                    platforms.array[3].x-=4;
                    gauchePlatform=false;
                    if(platforms.array[3].x<900){
                        gauchePlatform=true;
                    }
                }
                break;
            case 3:
                if(platforms.array[3].x<1600 && gauchePlatform==true){
                    platforms.array[3].x+=4;
                }else{
                    platforms.array[3].x-=4;
                    gauchePlatform=false;
                    if(platforms.array[3].x<1100){
                        gauchePlatform=true;
                    }
                }
                break;
            default:
                break;
        }

        if (checkCollision(player.rect, ground)) {
            player.rect.y = ground.y - player.rect.h;
            player.velY = 0;
            player.onGround = true;
        } else {
            // player.onGround = false;
            // mecanique du coyote time
            for (int i = 0; i < platforms.count; i++) {
                SDL_Rect plat = platforms.array[i];
                if (checkCollision(player.rect, plat) && player.velY >= 0 && player.rect.y <= plat.y - 30) {
                    if(i==3 && (level == 2 || level == 3)){
                        if(gauchePlatform==true){
                            player.rect.x+=12;
                        }else{
                            player.rect.x-=12;
                        }
                    }

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
                    if(buttons.array[i].texture==exitButton){
                            exit(0);
                    }else if(buttons.array[i].texture==backButton){
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
                            return startGame(0);
                            exit(0);
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
                    return startGame(level+1);
                    exit(0);
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

        if(level==3){
                plantScreen = plant.rect;
                plantScreen.x -= (int)cameraX;
                SDL_RenderCopy(renderer, plant.texture, NULL, &plantScreen);
                if (checkCollision(player.rect, plant.rect)) {
                    return startGame(level);
                }
                updatePlant(&plant, projectiles, projectileTexture, &player);
        }
        
        renderProjectiles(renderer, projectiles, cameraX);
        SDL_RenderCopy(renderer, character, NULL, &playerScreen);

        
        for (int i = 0; i < 10; i++) {
            if (projectiles[i].active && checkCollision(player.rect, projectiles[i].rect)) {
                return startGame(level);
            }
        }
        SDL_RenderPresent(renderer);

        
        updateProjectiles(projectiles);
        // if (keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_RIGHT]){
        //     SDL_Delay(10);
        // }
        // else{
            SDL_Delay(13);
        // }
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
    startGame(3);
}
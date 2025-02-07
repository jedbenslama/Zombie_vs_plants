#include <stdio.h>
#include <stdlib.h>

#define MAP_X 150
#define MAP_Y 5

#define ZOMBIE_X 0
#define ZOMBIE_Y MAP_Y - 1

void createMap(char map[MAP_Y][MAP_X]) {
    for (int i = 0; i < MAP_Y; i++) {
        for (int j = 0; j < MAP_X; j++) {
            map[i][j] = '_';
        }
    }
}

void showMap(char map[MAP_Y][MAP_X]) {
    for (int x = 0; x < MAP_X + 2; x ++) {
        printf("#");
    }
    printf("\n");
    for (int i = 0; i < MAP_Y; i++) {
        printf("#");
        for (int j = 0; j < MAP_X; j++) {
            if (j == ZOMBIE_X && i == ZOMBIE_Y) {
                printf("d");
            }
            else {
            printf("%c", map[i][j]);
            }
        }
        printf("#\n");
    }
    for (int x = 0; x < MAP_X +2; x ++) {
        printf("#");
    }
}

int main() {
    char map[MAP_Y][MAP_X];
    createMap(map);
    showMap(map);
    return 0;
}
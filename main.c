#include <stdio.h>
#include <stdlib.h>

#define MAP_X 150
#define MAP_Y 5

int z_x = 0; // zombie X
int z_y = MAP_Y - 1; // zombie Y

int *zombie_x = &z_x;
int *zombie_y = &z_y;

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
            if (j == *zombie_x && i == *zombie_y) { // faire spawn un "d" à (z_x, z_y)
                printf("d");
            }
            else {
            printf("%c", map[i][j]);
            }
        }
        printf("#\n");
    }
    for (int x = 0; x < MAP_X + 2; x++) {
        printf("#");
    }
}

int main() {
    char map[MAP_Y][MAP_X];
    createMap(map);
    showMap(map);
    return 0;
}
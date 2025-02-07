#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <conio.h>
    void sleepglobal(float tempsdesleep){
        sleep(tempsdesleep/1000);
    }
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    void sleepglobal(float tempsdesleep){
        sleep(tempsdesleep);
    }
    char getch(void) { // fonction getch car elle n'est pas sur mac (source: chatgpt)
        struct termios oldt, newt;
        char ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }

    int kbhit(void) { // fonction kbhit car elle n'est pas sur mac (source: chatgpt)
        struct termios oldt, newt;
        int ch;
        int oldf;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        if (ch != EOF) {
            ungetc(ch, stdin);
            return 1;
        }

        return 0;
    }
#endif

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
    printf("\n");
}

int main() {
    int test=0;
    while(1){
        char map[MAP_Y][MAP_X];
        createMap(map);
        showMap(map);
        test++;
        sleepglobal(1);
    }
    return 0;
}
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <string.h>


#define FAIL 0
#define SUCCESS 1
#define SIZE_X 11
#define SIZE_Y 11

char key;
COORD pos;
char board[SIZE_Y][SIZE_X];
int pos_list[100][2];
int length;
char vel = 'w';

void delay(int ms){
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not achieved
    while (clock() < start_time + ms);
}

void draw(){
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwWritten = 0;
    COORD mod_pos = pos;
    mod_pos.X *= 2;
    SetConsoleCursorPosition(out, mod_pos);
    WriteConsoleOutputCharacter(out, "P", 1, mod_pos, &dwWritten);
}

void fill_board(){
    int row, col;
    for (row = 0; row < SIZE_Y; row++){
        for (col = 0; col < SIZE_X; col++){
            if (row == pos.Y && col == pos.X){
                board[row][col] = 'P';
            } else {
                board[row][col] = '-';
            }
        }
    }
}

int move(){
    switch (key){
        case 'w':
            if (pos.Y > 0 && vel != 's'){
                pos.Y -= 1;
                vel = key;
            } else if (vel == 's'){
                key = vel;
                move();
            } else {
                return FAIL;
            }
            break;

        case 's':
            if (pos.Y < SIZE_Y - 1 && vel != 'w'){
                pos.Y += 1;
                vel = key;
            } else if (vel == 'w'){
                key = vel;
                move();
            } else {
                return FAIL;
            }
            break;

        case 'd':
            if (pos.X < SIZE_X - 1 && vel != 'a'){
                pos.X += 1;
                vel = key;
            } else if (vel == 'a'){
                key = vel;
                move();
            } else {
                return FAIL;
            }
            break;

        case 'a':
            if (pos.X > 0 && vel != 'd'){
                pos.X -= 1;
                vel = key;
            } else if (vel == 'd'){
                key = vel;
                move();
            } else {
                return FAIL;
            }
            break;

        default:
            key = vel;
            move();
            break;
    }
    return SUCCESS;
}

void init_board(){
    int row, col;
    for (row = 0; row < SIZE_Y; row++){
        for (col = 0; col < SIZE_X; col++){
            printf("%c ", board[row][col]);
        }
        printf("\n");
    }
}

void clear_board(){
    int row, col;
    COORD origin;
    origin.X = 0;
    origin.Y = 0;
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(out, origin);
    for (row = 0; row < SIZE_Y; row++){
        for (col = 0; col < SIZE_X; col++){
            printf("  ");
        }
        printf("\n");
    }
    SetConsoleCursorPosition(out, origin);
    printf("quit state\n");
}

int main(){
    system("cls");
    int success;
    pos.X = 5;
    pos.Y = 5;
    fill_board();
    init_board();
    delay(1000);
    while (1){
        if ( _kbhit() ){
            key = _getch();
            if (key == 'q')
                return 0;
            success = move();
            if (!success){
                goto quit;
            }
        } else {
            success = move();
            if (!success){
                goto quit;
            }
        }
        fill_board();
        draw();
        delay(300);
    }
    quit: clear_board();
          return 0;
}


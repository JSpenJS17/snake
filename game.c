#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <string.h>


#define TRUE 1
#define FALSE 0
#define SIZE_X 11
#define SIZE_Y 11

char key;
COORD pos;
char board[SIZE_Y][SIZE_X];


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

void move(){
    switch (key){
        case 'w':
            if (pos.Y > 0)
                pos.Y -= 1;
            break;

        case 's':
            if (pos.Y < SIZE_Y - 1)
                pos.Y += 1;
            break;

        case 'd':
            if (pos.X < SIZE_X - 1)
                pos.X += 1;
            break;

        case 'a':
            if (pos.X > 0)
                pos.X -= 1;
            break;

        default:
            break;
    }
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

int main(){
    system("cls");
    fill_board();
    init_board();
    while (1){
        if ( _kbhit() ){
            key = _getch();
            if (key == 'q')
                return 0;
            move();
            }
        else 
            move();
        fill_board();
        draw();
        delay(300);
    }
}


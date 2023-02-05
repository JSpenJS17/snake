/*
 * Author: Pierce Lane
 * Last modified: 2/5/2023
 * Made to test the limits of my ability
 *
 */

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
int snake_pos[100][2];
int del_pos[2];
int apple_pos[2];
char vel;



int rand_lim(int limit) {
    //return a random integer between 0 and the limit.
    int divisor = RAND_MAX/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

void delay(int ms){
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not achieved
    while (clock() < start_time + ms);
}

void clrinp(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void draw(int dead){
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD mod_pos;

    mod_pos.X = del_pos[0];
    mod_pos.Y = del_pos[1];
    mod_pos.X *= 2;
    SetConsoleCursorPosition(out, mod_pos);
    printf(" ");

    mod_pos.X = apple_pos[0];
    mod_pos.Y = apple_pos[1];
    mod_pos.X *= 2;
    SetConsoleCursorPosition(out, mod_pos);
    printf("\033[1;31ma");

    int i;
    for (i = 0; i < length; i++){
        mod_pos.X = snake_pos[i][0];
        mod_pos.Y = snake_pos[i][1];
        mod_pos.X *= 2;
        SetConsoleCursorPosition(out, mod_pos);
        if (!dead)
            printf("\033[1;32m");
        else
            printf("\033[1;31m");

        if (i == length - 1){
            switch (key){
                case 'w':
                    printf("^");
                    break;
                
                case 's':
                    printf("v");
                    break;

                case 'a':
                    printf("<");
                    break;

                case 'd':
                    printf(">");
                    break;
            }
        } else {
            printf("s");
        }
    }

    printf("\033[0m");
    //mod_pos.X = 0;
    //mod_pos.Y = 11;
    //SetConsoleCursorPosition(out, mod_pos);
    //printf("Length: %d", length);
    
}

int on_apple(){
    return (pos.X == apple_pos[0] && pos.Y == apple_pos[1]);
}

int on_snake(){
    int i;
    for (i = 0; i < length; i++){
        if (apple_pos[0] == snake_pos[i][0] && apple_pos[1] == snake_pos[i][1]){
            return 1;
        }
    }
    return 0;
}

void spawn_apple(){
    do {
        apple_pos[0] = rand_lim(SIZE_X-1);
        apple_pos[1] = rand_lim(SIZE_Y-1);
    } while (on_snake());
}

void add_pos(){
    int i;
    if (!on_apple()){
        del_pos[0] = snake_pos[0][0];
        del_pos[1] = snake_pos[0][1];
        for (i = 0; i < length-1; i++){
            snake_pos[i][0] = snake_pos[i+1][0];
            snake_pos[i][1] = snake_pos[i+1][1];
        }
        snake_pos[length-1][0] = pos.X;
        snake_pos[length-1][1] = pos.Y;
    
    } else {
        length++;
        snake_pos[length-1][0] = pos.X;
        snake_pos[length-1][1] = pos.Y;
        spawn_apple();
    }
}

int move(){
    switch (key){
        case 'w':
            if (pos.Y > 0 && vel != 's'){
                pos.Y -= 1;
                add_pos();
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
                add_pos();
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
                add_pos();
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
                add_pos();
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

    int i;
    for (i = 0; i < length-1; i++){
        if (pos.X == snake_pos[i][0] && pos.Y == snake_pos[i][1]){
            return FAIL;
        }
    }

    return SUCCESS;
}

void fill_board(){
    int row, col;
    for (row = 0; row < SIZE_Y; row++){
        for (col = 0; col < SIZE_X; col++){
            if (row == pos.Y && col == pos.X){
                board[row][col] = '^';
            } else {
                board[row][col] = ' ';
            }
        }
    }
}


void init_board(){
    int row, col;
    for (row = 0; row < SIZE_Y; row++){
        for (col = 0; col < SIZE_X; col++){
            if (board[row][col] == '^')
                printf("\033[1;32m%c \033[0m", board[row][col]);
            else
                printf("%c ", board[row][col]);
        }
        if (row < SIZE_Y-1)
            printf("\n");
    }
    spawn_apple();
}

int good_input(char input, char *options){
    int i = 0;
    while (*(options + i)){
        if (input == options[i])
            return 1;
        i++;
    }
    return 0;
}

int ask_play_again(char *mode){
    char go_again;
    do {
        system("cls");
        printf("Game over!\n\n");
        printf("Mode: %s\n", mode);
        printf("Final Length: %d\n", length);
        printf("\nPlay again? (y/n): ");
        go_again = getchar();
    } while (go_again != 'y' && go_again != 'n');
    clrinp();

    if (go_again == 'y')
        return SUCCESS;
    else
        return FAIL;
}

int key_in_wasd(char key){
    int i;
    char wasd[] = {'w', 'a', 's', 'd'};
    for (i = 0; i < 4; i++){
        if (key == wasd[i])
            return SUCCESS;
    }
    return FAIL;
}

int main(){
    system("MODE 22,11");
    start: system("cls");
    int success, speed;
    char choice = ' ';
    length = 1;

    char options[] = {'1', '2', '3', '4'};
    while (!good_input(choice, options)) {
        printf("Welcome to Snake!\n");
        printf("By Pierce Lane\n");
        printf("WASD to move.\n\n");
        printf("Select a speed:\n");
        printf("1) Slow\n2) Medium\n3) Fast\n4) Insane\n");
        printf("Enter a number: ");

        choice = getchar();
        clrinp();
        system("cls");
    }

    char mode[10];
    switch (choice){
        case '1':
            strcpy(mode, "Easy");
            speed = 400;
            break;
        case '2':
            strcpy(mode, "Medium");
            speed = 300;
            break;
        case '3':
            strcpy(mode, "Hard");
            speed = 200;
            break;
        case '4':
            strcpy(mode, "Insane");
            speed = 100;
            break;
    }

    system("cls");

    printf("\e[?25l");
    
    srand(time(NULL));
    pos.X = 5;
    pos.Y = 5;
    del_pos[0] = pos.X;
    del_pos[1] = pos.Y;
    fill_board();
    init_board();

    do {
        while (!_kbhit());
        key = _getch();
        vel = key;
    } while (!key_in_wasd(key));

    draw(0);

    while (1){
        if ( _kbhit() ){
            key = _getch();
            success = move();
            if (!success){
                draw(1);
                break;
            }
        } else {
            success = move();
            if (!success){
                draw(1);
                break;
            }
        }
        fill_board();
        draw(0);
        delay(speed);
    }
    delay(1000);

    printf("\e[?25h");
    if (ask_play_again(mode)){
        goto start;
    }

    printf("\nThanks for playing!\n");
    printf("Made by: Pierce Lane\n");
    delay(2000);
    return 0;
}


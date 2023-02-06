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
#include <wchar.h>
#include <locale.h>

//define constants
#define FAIL 0
#define SUCCESS 1
#define SIZE_X 11
#define SIZE_Y 11

//define static variables
char key;
COORD pos;
int length;
int snake_pos[SIZE_X * SIZE_Y + 1][3];
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
    //delay a millisecond amount
    //storing start time
    clock_t start_time = clock();
 
    //looping till desired time is achieved
    while (clock() < start_time + ms);
}

void clrinp(){
    //helper function to clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void draw(int dead){
    //draws the next frame
    //set the output handle to the current terminal output
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    //create a COORD variable for determining where to print
    COORD mod_pos;

    //first delete the square set to be deleted
    //set mod_pos.X
    mod_pos.X = del_pos[0];
    //adjust mod_pos.Y to account for top of the border
    mod_pos.Y = del_pos[1]+1;
    //adjust mod_pos.X to account for border pillar and extra spaces in printout
    mod_pos.X = mod_pos.X * 2 + 2;
    //set the cursor position to the mod_pos
    SetConsoleCursorPosition(out, mod_pos);
    //print the desired character there
    printf(" ");

    //then print the apple
    mod_pos.X = apple_pos[0];
    mod_pos.Y = apple_pos[1]+1;
    mod_pos.X = mod_pos.X * 2 + 2;
    SetConsoleCursorPosition(out, mod_pos);
    printf("\033[1;31ma");

    //then loop through and reprint the whole snake
    //this may seem inefficient, but this allows more customization down the
    //line for very little real performance impact
    int i;
    for (i = 0; i < length; i++){
        mod_pos.X = snake_pos[i][0];
        mod_pos.Y = snake_pos[i][1]+1;
        mod_pos.X = mod_pos.X * 2 + 2;
        SetConsoleCursorPosition(out, mod_pos);

        //if the snake isn't dead
        if (!dead)
            //we print it as green
            printf("\033[1;32m");
        //if the snake is dead
        else
            //we print it as red
            printf("\033[1;31m");
        
        //print the head of the snake.
        //changes directions depending on the stored direction in the head
        if (i == length - 1){
            switch (snake_pos[length-1][2]){
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
        //print the tail of the snake, vice versa directionally as head
        } else if (i == 0){
            switch (snake_pos[0][2]){
                case 'w':
                    printf("v");
                    break;
                
                case 's':
                    printf("^");
                    break;

                case 'a':
                    printf(">");
                    break;

                case 'd':
                    printf("<");
                    break;
            }

        //print the body of the snake
        } else {
            printf("~");
        }
    }

    printf("\033[0m");
}

int on_apple(){
    //checks if snake head is on an apple
    return (pos.X == apple_pos[0] && pos.Y == apple_pos[1]);
}

int on_snake(){
    //checks if apple position is on the snake
    int i;
    for (i = 0; i < length; i++){
        if (apple_pos[0] == snake_pos[i][0] && apple_pos[1] == snake_pos[i][1]){
            return 1;
        }
    }
    return 0;
}

void spawn_apple(){
    //sets the new apple_pos to a valid position
    do {
        apple_pos[0] = rand_lim(SIZE_X-1);
        apple_pos[1] = rand_lim(SIZE_Y-1);
    } while (on_snake());
}

void add_pos(){
    int i;
    //if we're not on an apple
    if (!on_apple()){
        //mark the kaboose for deletion during drawing
        del_pos[0] = snake_pos[0][0];
        del_pos[1] = snake_pos[0][1];
        //shift everything left one to account for newly missing 0th element
        for (i = 0; i < length-1; i++){
            //assign snake_pos[i] to snake_pos[i+1]
            snake_pos[i][0] = snake_pos[i+1][0];
            snake_pos[i][1] = snake_pos[i+1][1];
            snake_pos[i][2] = snake_pos[i+1][2];
        }
        //add the position of the new head to the back of the list to account
        //for newly missing length-1th element
        snake_pos[length-1][0] = pos.X;
        snake_pos[length-1][1] = pos.Y;
        //also add what direction it was going
        snake_pos[length-1][2] = key;
    
    //if we're on an apple
    } else {
        //increment length
        length++;
        //update pos as before
        snake_pos[length-1][0] = pos.X;
        snake_pos[length-1][1] = pos.Y;
        snake_pos[length-1][2] = key;
        //and spawn a new apple
        spawn_apple();
    }
}

int move(){
    //updates snake pos depending on key pressed
    switch (key){
        case 'w':
            //if in bounds and not going south currently
            if (pos.Y > 0 && vel != 's'){
                //go up one
                pos.Y -= 1;
                //add this new position to the position list
                add_pos();
                //remember that we're going north
                vel = key;

            //if we were going south
            } else if (vel == 's'){
                //set key to going south
                key = vel;
                //redo the move and continue south
                move();

            //if we were out of bounds
            } else {
                //return that we failed the move check
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
            //this is more of a failsafe than anything, should never be executed
            //if no key was given, set key to previously pressed key
            key = vel;
            //move in that direction
            move();
            break;
    }

    //check if snake head is on snake body
    int i;
    for (i = 0; i < length-1; i++){
        //if snake head is on the body
        if (pos.X == snake_pos[i][0] && pos.Y == snake_pos[i][1]){
            //return that we failed the move check
            return FAIL;
        }
    }

    //if we made it here, return that the move was good!
    return SUCCESS;
}

void print_bar(){
    //prints a horizontal bar the width of the play field
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(out, 0x88);

    int i;
    for (i = 0; i <= SIZE_X * 2 + 2; i++)
        printf("-");

    SetConsoleTextAttribute(out, 0x07);

}

void print_poles(){
    //prints the two vertical bars of the border as well as the initial snake
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    
    int i, j;
    for (i = 0; i < SIZE_Y; i++){
        SetConsoleTextAttribute(out, 0x88);
        printf(". ");
        
        SetConsoleTextAttribute(out, 0x07);
        for (j = 0; j < SIZE_X*2 - 1; j++)
            if (i == pos.Y && j/2 == pos.X){
                printf("\033[1;32m^ \033[0m");
                j++;
            } else {
                printf(" ");
            }
        
        SetConsoleTextAttribute(out, 0x88);
        printf("11");

        SetConsoleTextAttribute(out, 0x07);
        printf("\n");
    }
}

void init_board(){
    //print the top bar
    print_bar();
    printf("\n");

    //print the two poles and initial snake
    print_poles();

    //print the bottom bar
    print_bar();

    //spawn the first apple
    spawn_apple();
}

int is_good_input(char input, char *options){
    //checks if char input is in char[] options. Returns 1 if so, 0 if not.
    int i = 0;
    while (*(options + i)){
        if (input == options[i])
            return 1;
        i++;
    }
    return 0;
}

int ask_play_again(char *mode){
    //asks the user if they'd like to play again. returns 1 if they do, else 0.
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
    //returns 1 if the char given is in the char array "wasd"
    int i;
    char wasd[] = {'w', 'a', 's', 'd'};
    for (i = 0; i < 4; i++){
        if (key == wasd[i])
            return SUCCESS;
    }
    return FAIL;
}

int main(){
    //resize window to perfect snakey size
    system("MODE 25,13");
    
    //create a label for easy restarting and clear screen
    start: system("cls");
    //success determines if a move worked, speed is the delay in ms of the run
    int success, speed;
    //choice is the player's choice of menu item (1, 2, 3)
    char choice = ' ';
    //set the length to 1 to begin with
    length = 1;

    //main menu
    char options[] = {'1', '2', '3', '4'};
    while (!is_good_input(choice, options)) {
        printf("Welcome to Snake!\n");
        printf("By Pierce Lane\n");
        printf("WASD to move.\n\n");
        printf("Select a speed:\n");
        printf("1) Slow\n2) Fast\n3) Insane\n4) ULTRA NIGHTMARE\n");
        printf("Enter a number: ");

        choice = getchar();
        clrinp();
        system("cls");
    }

    //set speed depending on what choice was
    char mode[20];
    switch (choice){
        case '1':
            strcpy(mode, "Slow");
            speed = 300;
            break;
        case '2':
            strcpy(mode, "Fast");
            speed = 200;
            break;
        case '3':
            strcpy(mode, "Insane");
            speed = 100;
            break;
        case '4':
            strcpy(mode, "ULTRA NIGHTMARE");
            speed = 50;
            break;
    }

    //clear the menu
    system("cls");

    //turn off the cursor
    printf("\e[?25l");
    
    //get a random seed
    srand(time(NULL));
    //set starting position
    pos.X = 5;
    pos.Y = 5;
    //set starting position to be deleted on next draw
    del_pos[0] = pos.X;
    del_pos[1] = pos.Y;
    //draw the board border and initial snake head
    init_board();

    //wait until we get a WASD input then go
    do {
        while (!_kbhit());
        key = _getch();
        vel = key;
    } while (!key_in_wasd(key));

    //draw with an alive snek
    draw(0);


    //main game loop
    while (1){
        //if a key was hit
        if ( _kbhit() ){
            //grab the key
            key = _getch();
            //move and store it's success state in success
            success = move();
            //if the move failed, draw the dead snek and break
            if (!success){
                draw(1);
                break;
            }

        //if no key was hit
        } else {
            //move without updating key pressed, same as before
            success = move();
            if (!success){
                draw(1);
                break;
            }
        }
        //update display with an alive snek
        draw(0);
        //delay depending on chosen speed
        delay(speed);
    }
    //after death, wait 1 second to let it sink in
    delay(1000);

    //turn back on the cursor
    printf("\e[?25h");
    //ask the player to play again
    if (ask_play_again(mode)){
        //if they say yes, restart
        goto start;
    }

    //otherwise, thank the user for playing and do credits
    printf("\e[?25l"); //remove the cursor again
    printf("\nThanks for playing!\n");
    printf("Made by: Pierce Lane\n\n");
    printf("Special thanks to:\n");
    printf("Landrea\n");
    delay(2000);
    //also resize their window to a usable size for if they ran this in terminal
    system("MODE 120,30");
    return 0;
}


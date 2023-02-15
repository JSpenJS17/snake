/*
 * Author: Pierce Lane
 * Last modified: 2/14/2023
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
#define UP 72
#define LEFT 75
#define DOWN 80
#define RIGHT 77

//define static variables
int SIZE_X = 11;
int SIZE_Y = 11;
char key;
COORD pos;
int length;
int snake_pos[19 * 19 + 1][3];
int del_pos[2];
int apple_pos[2];
char vel;
int style = 0;

int menu(char **options, int height_offset, int menu_length, int options_on,
        char* header);

void clear_screen(){
    //function for clearing the std output screen
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD origin;
    origin.X = 0;
    origin.Y = 0;
    SetConsoleCursorPosition(out, origin);
    printf("\033[0J");
}

char* centerText(char *text, int text_length, int buffer_len) { 
    //return string text centered with spaces given the buffer length
    int fieldWidth = buffer_len * 2 + 3;
    int padlen = (fieldWidth - strlen(text)) / 2;
    static char output[30];

    int i;
    for (i = 0; i < padlen; i++)
        output[i] = ' ';
    
    for (i = i; i < text_length; i++)
        output[i] = text[i];

    for (i = i; i < padlen; i++)
        output[i] = ' ';

    snprintf(output, 30, "%*s%s%*s\n", padlen, "", text, padlen, "");

    return output;
}

void resize_window(int x, int y){
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "MODE %d,%d", x, y);
    system(buffer);
}

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

void print_directional_segment(int seg_index){
    switch (snake_pos[seg_index][2]){
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
}

void print_reverse_directional_segment(int seg_index){
    switch (snake_pos[seg_index][2]){
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
        
        //print the snake
        switch (style){
            //changes directions depending on the 
            //stored direction in the segment
            case 0:
                print_directional_segment(i);
                break;

            case 1:
                if (i == length - 1)
                    print_directional_segment(i);
                else if (i != 0)
                    printf("~");
                else
                    print_reverse_directional_segment(i);
                break;

            case 2:
                if (i == length - 1)
                    print_directional_segment(i);
                else
                    printf("~");
                break;
        }
    }
        
    
    //restore color to black
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

void convert_key(){
    //if the key entered was an arrow key, two inputs are sent: -32 and
    //the value of the arrow key. if the input is on -32, we need to 
    //skip it by calling _getch() again.
    if (key == -32)
        key = _getch();

    //convert arrow key inputs to wasd
    switch (key){
        case UP:
            key = 'w';
            break;

        case DOWN:
            key = 's';
            break;

        case RIGHT:
            key = 'd';
            break;

        case LEFT:
            key = 'a';
            break;
    }
}

int move(){
    //updates snake pos depending on key pressed

    //convert arrow key input to wasd
    convert_key();

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

int key_in_wasd(char key){
    //returns 1 if the char given is in the char array "wasd"
    int i;
    int wasd[] = {'w', 'a', 's', 'd', UP, DOWN, LEFT, RIGHT};
    for (i = 0; i < 8; i++){
        if (key == wasd[i])
            return SUCCESS;
    }
    return FAIL;
}

void death_animation(){
    //handle the death wiping animation
    int i, j;
    COORD cursor_pos;
    cursor_pos.X = 0;
    cursor_pos.Y = 0;
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(out, 0x00);
    SetConsoleCursorPosition(out, cursor_pos);

    char str[SIZE_X*2 + 3];
    str[0] = '.';
    for (j = 1; j < SIZE_X*2 + 3; j++){
        str[j] = ' ';
    }

    for (i = 0; i < SIZE_Y+2; i++){
        cursor_pos.Y = i;
        SetConsoleCursorPosition(out, cursor_pos);
        printf("%s", str);    
        delay(50);
    }

    SetConsoleTextAttribute(out, 0x07);
    cursor_pos.X = 0;
    cursor_pos.Y = 0;
    SetConsoleCursorPosition(out, cursor_pos);
}

void options_menu(){
    //create a page number tracker
    int page = 0;
    while (page < 2 && page >= 0){
        //clear the screen every time
        clear_screen();

        //reset key every time we're here so that the errant \n key doesn't
        //close the menu before we've selected what we want
        key = 12;

        //create a choice integer
        int choice;

        if (page == 0){
            //do board size option on page 0
            char *sizes[] = {"11x11 (default)", "13x13", "15x15",
                            "17x17", "19x19", "Next", "Back"};
            choice = menu(sizes, 2, 7, 0, "\033[1;32mBoard Size:\033[0m\n");
            int choices[] = {11, 13, 15, 17, 19, SIZE_X, SIZE_X};
            SIZE_X = choices[choice];
            SIZE_Y = SIZE_X;
            if (choice == 6)
                //if choice was Back, go back a page
                page--;
            else
                //if choice wasn't back, go forward
                page++;

        } else if (page == 1){
            char *styles[] = {"<<<<<", "<~~~>", "<~~~~", "Next", "Back"};
            choice = menu(styles, 2, 5, 0, "\033[1;35mSnake Style:\033[0m\n");
            if (choice <= 2){
                //if they chose a style
                style = choice;
                page++;
            } else if (choice == 3)
                //if they chose Next
                page++;
            else
                //if they chose Back
                page--;
        }
    }

    //finally, after the user exited the options menu, clear the screen
    clear_screen();
}


int main_menu(){
    //choice is the player's choice of menu item (1, 2, 3, 4)
    int choice = 0;
    //make the header, which is just some formatted text printed out at the
    //beginning of the menu
    char header[134] = "\033[1;32m";
    strcat(header, centerText("S N A K E", 9, 11));
    
    //change color back and print menu stuff
    strcat(header, "\n\033[0mMade by Pierce Lane\n\nWASD/Arrow Keys to move");
    strcat(header, "\nEscape for options");
    strcat(header, "\nEnter to select\n\nSelect a difficulty:\n");

    //main menu options
    char *options[] = {"\033[1;32mSlow\033[0m",
                     "\033[1;33mFast\033[0m",
                     "\033[1;31mInsane\033[0m",
                     "\033[1;35mULTRA NIGHTMARE\033[0m"};
    
    //get the choice from the menu
    choice = menu(options, 10, 4, 1, header);

    //turn off the cursor
    printf("\e[?25l");
    
    return choice;
}

int menu(char **options, int start_line, int menu_length, int options_on,
        char *header){
    //make sure the cursor is on
    printf("\e[?25h");

    //print the header
    printf("%s", header);

    //define menu choice number
    int choice = 0;

    //get terminal settings and stuff
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD menu_pos;
    menu_pos.X = 0;
    menu_pos.Y = start_line - 1;

    //print menu
    int i;
    for (i = 0; i < menu_length; i++){
        printf("%d) %s\n", i+1, options[i]);
        
    }

    //do interactive menu part
    SetConsoleCursorPosition(out, menu_pos);
    //handle cursor position and selection
    while (key != 13) {
        if (_kbhit()){
            key = _getch();
            //convert arrow key inputs to wasd
            convert_key();
            switch (key){
                case 's':
                    if (choice < menu_length - 1)
                        choice++;
                    else
                        choice = 0;
                    break;
                case 'w':
                    if (choice > 0)
                        choice--;
                    else
                        choice = menu_length - 1;
                    break;
                //if user hits escape
                case 27:
                    //if the options are enabled for this menu
                    if (options_on){
                        //display options menu
                        options_menu();
                        //after options are set, print header and menu again
                        printf("%s", header);
                        for (i = 0; i < menu_length; i++)
                            printf("%d) %s\n", i+1, options[i]);
                        //also set key != 13
                        key = 12;
                    }
                    break;
            }
        menu_pos.Y = start_line + choice - 1;    
        SetConsoleCursorPosition(out, menu_pos);
        }
    }

    //reset cursor position to 0, 0
    menu_pos.Y = 0;
    SetConsoleCursorPosition(out, menu_pos);
    return choice;
}

int main(){
    //get terminal information
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    //get the size of the screen before we mess it up
    CONSOLE_SCREEN_BUFFER_INFO size;
    GetConsoleScreenBufferInfo(out, &size);

    //resize window to perfect snakey size
    resize_window(11 * 2 + 3, 11 + 2);
 
    //create the go_again while loop for if user wants to play again
    int go_again = 1;
    while (go_again){
        //clear the screen
        clear_screen();

        //variable success determines if a move worked
        int success;   

        //variable speed is the delay in ms of the run
        int speed;

        //variable choice for storing menu option chosen by user
        int choice;

        //set the length to 1 to begin with
        length = 1;

        //set key to 0 to begin with
        key = 0;

        //print main menu and get choice of speed from there
        choice = main_menu();
        //set speed depending on choice
        char mode[33];
        switch (choice){
            case 0:
                strcpy(mode, "\033[1;32mSlow\033[0m");
                speed = 300;
                break;
            case 1:
                strcpy(mode, "\033[1;33mFast\033[0m");
                speed = 200;
                break;
            case 2:
                strcpy(mode, "\033[1;31mInsane\033[0m");
                speed = 100;
                break;
            case 3:
                strcpy(mode, "\033[1;35mULTRA NIGHTMARE\033[0m");
                speed = 50;
                break;
        }

        //resize the window to set dimensions from options menu
        SIZE_Y = SIZE_X;
        resize_window(SIZE_X * 2 + 3, SIZE_Y + 2);

        //get a random seed
        srand(time(NULL));

        //set starting position
        pos.X = SIZE_X / 2;
        pos.Y = SIZE_Y / 2;

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
        //after death, play death animation
        delay(500);
        death_animation();
        
        //resize the window back to normal
        resize_window(11 * 2 + 3, 11 + 2);

        //change color and print Game Over centered
        printf("\033[1;31m");
        printf("%s", centerText("Game Over", 9, 11));;

        //change color back and print stats + menu options
        printf("\033[0m\n");
        printf("Mode: %s\n", mode);
        printf("Board Size: %dx%d\n", SIZE_X, SIZE_Y);
        printf("Final Length: %d\n\n", length);
        printf("Play again?\n");

        //ask the player if they'd like to play again
        char *yesno[] = {"\033[1;32mYes\033[0m", "\033[1;31mNo\033[0m"};
        //go_again doesn't need any header option because it's all already
        //printed. this saves some memory, not having to store the output str.
        go_again = !menu(yesno, 8, 2, 0, "");
    }
        

    //otherwise, thank the user for playing and do credits
    clear_screen();
    printf("\e[?25l"); //remove the cursor again
    //change color
    printf("\033[1;32m");
    //print Thanks for playing! centered
    printf("%s", centerText("Thanks for playing!", 19, 11));
    //change color back
    printf("\n\033[0m");
    printf("Made by:\n");
    printf("Pierce Lane\n\n");
    printf("Special thanks to:\n");
    printf("Landrea\n\n");
    printf("Press any key to exit...");
    getch();
    //also resize their window to original size
    resize_window(size.dwMaximumWindowSize.X, size.dwMaximumWindowSize.Y-1);
    return 0;
}


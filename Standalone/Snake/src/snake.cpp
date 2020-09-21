/*******************************************************
PROJECT NAME - SnakeGameforWindows

Copyright © 2020-2022 Jeffery Yang

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*********************************************************/
/************************ MANUAL *************************

Traditional snake game for the Windows command line.
Use the arrow keys to move.
Gain points and a longer body from eating eggs.
But the snake dies if it touches a wall or its own body!
Win condition? A body as large as the game board itself!

*********************************************************/

/********************* VERSION INFO **********************
09/21 - Version 1.0. Barebone snake game;
      - menu ontainers start / exit game options
*********************************************************/

/*********************** DEV HUB *************************
TAGS:
#ConsoleDevelopment, #Windows, #Linux

FUTURE FEATURES:
1) Score counter
2) Bonus egg points
3) Linux release
4) Snake speed / board size parameter adjustment
5) Scores table
*********************************************************/

#include <iostream>
#include <Windows.h>
#include <string>
#include <stdlib.h>
#include <cassert>
#include <list>
#include <vector>
#include <random>
#include <conio.h>
#include <chrono>
#include "../lib/Common/Common.h"



#define QUEUE std::list
#define min_selection 1
#define max_selection 2
#define INITIALIZE 0
#define STARTGAME 1
#define EXITGAME 2



enum class DIRECTION {UP, RIGHT, DOWN, LEFT};       // simplifying integer directions into "DIRECTION" class



class Egg;                                          //  Egg / Snake declaration for declaring as "friend" in gameBoard
class Snake;

void displayMenu() {                                // displays menu on startup
    std::cout << "\n\n          SNAKE          \n\n";
    std::cout << "        1. Start game              \n";
    std::cout << "        2. Exit game               \n\n\n\n";
    std::cout << "           Please make your selection: ";
}

class gameBoard {
    HANDLE hStdout;                     // Windows "HANDLE" for stdOut
    CONSOLE_SCREEN_BUFFER_INFO Board_p; // screen buffer info for windows console window
    std::mt19937 rand_eng;              // 1 board, 1 engine, 1 seed; this is what make_new_egg method "samples"
    int BOARD_TOP_BORDER;               // top border y coordinate
    int BOARD_LEFT_BORDER;              // left border x coordinate
    int BOARD_BOTTOM_BORDER;            // bottom border y coordinate
    int BOARD_RIGHT_BORDER;             // right border x coordinate
    int score;                          // current score
    int maxscore;                       // max score; equal to size of total gameboard less borders
public:
    gameBoard();

    friend Egg;                         //  both Egg and Snake need access to gameBoard member variables
    friend Snake;

    void initialize_Board_Obj(Snake& ini_Snake, Egg& ini_Egg);                                      // initializes board state with egg and snake.
    int print_initial_Board(Snake& ini_Snake, Egg& ini_Egg);                                        // displays initial board in console window. 1 if successful.
    int update_Objects(DIRECTION& new_dir, Snake& snake_PC, Egg& egg_PC);                           // updates object parameters / cycle. 0 if game continues; 1 if loss; 2 if win.
    void draw_Screen(Snake snake_PC, Egg egg_PC, COORD old_Head, COORD old_Tail, bool egg_eaten);   // displays board after each cycle.
    int getScore() { return (score); };                                                             // returns current score
};

class Egg {
    COORD current_Position;                                  // current egg COORD
    COORD make_new_Egg(COORD& snake_Head, gameBoard& env);   // generates new COORD for current_Position. Cannot be snake_Head (but it can be snake_Body) or borders. Uses Board.rand_eng to seed.
    void update_Egg(COORD& new_Egg);                         // updates Egg with COORD generated from make_new_Egg
public:
        // structors (Egg)
    Egg(): current_Position({-1, -1}) { };                   // CTOR

    friend gameBoard;                                        // gameBoard::update_Objects needs access to update_Egg
    void draw_new_Egg(COORD& snake_Head, gameBoard& env);    // draws new egg (called AFTER make_new_egg & update_Egg is called)

};

class Snake {
    int length;                                 // # of segments, including the head
    COORD Head;                                 // represents CURRENT head COORD
    COORD Tail;                                 // represents PREVIOUS tail COORD
    void initialize_Snake(COORD new_COORD);     // initialize_Snake sets head / tail to middle of screen.

public:
            // structors (Snake)
    Snake(): length(1), pivot_Cont(*(new QUEUE<COORD>)) {};

    friend gameBoard;                                                           // gameBoard::initialize_Board_Obj needs access to initialize_Snake and length;  other methods need access to length.
    QUEUE<COORD> pivot_Cont;                                                    // represents "pivot" coordinates in the snake's body. Pivot defined as "corner" of snake or the tail coord
    int update_Head(DIRECTION& new_dir, gameBoard& env);                        // updates position of head. TRUE if head updates successfully.
    int update_Snake(DIRECTION& new_dir, COORD old_Head, bool egg_eaten);       // updates position of snake. TRUE if updates successfully.
    COORD getHead() { return Head; };                                           // returns Head COORD
};

gameBoard::gameBoard() : hStdout(GetStdHandle(STD_OUTPUT_HANDLE)), score(0) {

                // Implementation-friendly way to seed random_engine

        std::random_device rd;
        unsigned seed;
        if (rd.entropy())
                // ... if random_device works, then we set seed accordingly.
           seed = rd();
        else
                // otherwise: no random_device available, seed using the system clock
           seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());

        rand_eng.seed(seed);


           // extract ConsoleScreenBuffer
    GetConsoleScreenBufferInfo(hStdout, &Board_p);

          // Obtaining max window size aprameters

      COORD max_WindowSize = {GetLargestConsoleWindowSize(hStdout).X, GetLargestConsoleWindowSize(hStdout).Y};
      _SMALL_RECT MaxWindowSize;                      // declaring a RECT to store new window
      MaxWindowSize.Left = 0;
      MaxWindowSize.Top = 0;
      MaxWindowSize.Right = max_WindowSize.X*2/3;     // right and bottom determines how large the board is; testing shows a typical 15' monitor
      MaxWindowSize.Bottom = max_WindowSize.Y*4/5;    // will probabably be too large. Reduce down to something like half the maxwindowsize.


            // Reset windows buffer, then reset console windows size
      SetConsoleScreenBufferSize(hStdout, {MaxWindowSize.Right, MaxWindowSize.Bottom });        // this sets Board_p.dwMaximumWindowSize

      MaxWindowSize.Right -= 1;                                    // display window size must be < buffer size
      MaxWindowSize.Bottom -= 1;                                   // display window size must be < buffer size

      SetConsoleWindowInfo(hStdout, 1, &MaxWindowSize);            // this sets Board_p.srWindow
      GetConsoleScreenBufferInfo(hStdout, &Board_p);               // extract screenbuffer info.


            // Setting gameboard borders based on csbi info (with assert, if possible)

        BOARD_TOP_BORDER = 2;                                      // "2" instead of "0" to allow for "heads up display"
        BOARD_LEFT_BORDER = 0;
        BOARD_BOTTOM_BORDER = Board_p.srWindow.Bottom;
        BOARD_RIGHT_BORDER = Board_p.srWindow.Right;

        maxscore = (BOARD_RIGHT_BORDER - BOARD_LEFT_BORDER-1)*(BOARD_TOP_BORDER - BOARD_BOTTOM_BORDER-1);
}

int gameBoard::print_initial_Board(Snake& ini_Snake, Egg& ini_Egg) {

                // make cursor invisible
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hStdout, &cursorInfo);         // retrieves cursor info
    cursorInfo.bVisible = 0;                            // set the cursor visibility
    SetConsoleCursorInfo(hStdout, &cursorInfo);         // attaches cursor info to output device


    DWORD cCharsWritten;

   // Bottom wall

    FillConsoleOutputCharacter( hStdout, (TCHAR)'#', Board_p.srWindow.Right, {BOARD_LEFT_BORDER, BOARD_BOTTOM_BORDER}, &cCharsWritten );

   // Top wall

    FillConsoleOutputCharacter( hStdout, (TCHAR)'#', Board_p.srWindow.Right, {BOARD_LEFT_BORDER, BOARD_TOP_BORDER}, &cCharsWritten);

   // Left wall + right wall

   for (int i = 2; i <= Board_p.srWindow.Bottom; i++) {
     FillConsoleOutputCharacter( hStdout, (TCHAR)'#', 1, {BOARD_LEFT_BORDER, static_cast<SHORT>(i)}, &cCharsWritten);          // Left wall
     FillConsoleOutputCharacter( hStdout, (TCHAR)'#', 1, {BOARD_RIGHT_BORDER, static_cast<SHORT>(i)}, &cCharsWritten);         // Right wall
   };

    // Print initial snake head
    FillConsoleOutputCharacter( hStdout, (TCHAR)'@', 1, ini_Snake.Head, &cCharsWritten);

    // Print egg initial position

    ini_Egg.draw_new_Egg(ini_Snake.Head, *this);

    return (1);
}

int gameBoard::update_Objects(DIRECTION& new_dir, Snake& snake_PC, Egg& egg_PC) {

    COORD oldHead = snake_PC.Head;
    COORD oldTail = snake_PC.Tail;

    bool egg_eaten = (snake_PC.Head.X == egg_PC.current_Position.X && snake_PC.Head.Y == egg_PC.current_Position.Y);    //tested; works.

            // Snake update
    if (snake_PC.update_Head(new_dir, *this)) {                            // collision on '0'; updated successfully on 1.
        if (!snake_PC.update_Snake(new_dir, oldHead, egg_eaten)) {         // collision on '0'; updates successfully on 1.
            return(1);                                                     // if either updates fail (return 0), we terminate by returning 1 to indicate loss.
        }
    }
    else{
        return(1);
    }

            // Egg update
    if (egg_eaten){
        score++;                                                         // +1 to score
        if (score == maxscore)                                           // check for win condition here.
            return(2);

        COORD newEgg = egg_PC.make_new_Egg(snake_PC.Head, *this);        // randomly generating new egg.
        egg_PC.update_Egg(newEgg);                                       // updates egg COORD
    }

            // redraw screen
    draw_Screen(snake_PC, egg_PC, oldHead, oldTail, egg_eaten);          // redrawing window after updates.

    return (0);                                                          // returns 0 to terminate condition -> continue next cycle.
}

void gameBoard::draw_Screen(Snake snake_PC, Egg egg_PC, COORD old_Head, COORD old_Tail, bool egg_eaten) {

    DWORD cCharsWritten;

            // Redrawing Snake body:

    if (!egg_eaten) {

            // Clear old tail

        if (!(old_Tail.X == egg_PC.current_Position.X && old_Tail.Y == egg_PC.current_Position.Y))         // for minor case where egg egg spawns on body, and tail is about to clear it.
            FillConsoleOutputCharacter( hStdout, (TCHAR)' ', 1, old_Tail, &cCharsWritten);

        if (snake_PC.length > 1)
            FillConsoleOutputCharacter( hStdout, (TCHAR)'X', 1, old_Head, &cCharsWritten );

    }

    else {  // egg is eaten

            // Draw new egg
        FillConsoleOutputCharacter( hStdout, (TCHAR)'0', 1, egg_PC.current_Position, &cCharsWritten );

            // Draw new neck
        FillConsoleOutputCharacter( hStdout, (TCHAR)'X', 1, old_Head, &cCharsWritten );
    };

            // Print new head.
        FillConsoleOutputCharacter( hStdout, (TCHAR)'@', 1, snake_PC.Head, &cCharsWritten );




};

void Snake::initialize_Snake(COORD new_Snake) {
    Head = new_Snake;
    Tail = new_Snake;
};

void gameBoard::initialize_Board_Obj(Snake& ini_Snake, Egg& ini_Egg) {
    ini_Snake.initialize_Snake({Board_p.dwMaximumWindowSize.X/2 , Board_p.dwMaximumWindowSize.Y/2});        // snake head initialized at center of board
    ini_Egg.make_new_Egg(ini_Snake.Head, *this);
}

int Snake::update_Head(DIRECTION& new_dir, gameBoard& env) {

        // updating current_Head.

    if (new_dir == DIRECTION::UP)
        --Head.Y;
    else if (new_dir == DIRECTION::DOWN)
        ++Head.Y;
    else if (new_dir == DIRECTION::RIGHT)
        ++Head.X;
    else if (new_dir == DIRECTION::LEFT){
        --Head.X;
    };

            // wall collision check; tested.
    if ( Head.X == env.BOARD_LEFT_BORDER || Head.Y == env.BOARD_TOP_BORDER || Head.X == env.BOARD_RIGHT_BORDER || Head.Y == env.BOARD_BOTTOM_BORDER ) {
        return(0);
    }

    return(1);
}

int Snake::update_Snake(DIRECTION& new_dir, COORD old_Head, bool egg_eaten) {

        // making preprocessing parameters (we need THREE "pivot" pointers for this algo to work: prev_pivot --> pivot_It, and a "middleman"
        // to store pivot_It between iterations)

    COORD prev_pivot;                                       // will be initialized to 'Head' in the main loop below; redundant to set twice
    COORD next_pivot = Head;                                // initialized to Head but 1st iteration will set it to 1st pivot AFTER Head
                                                            // [OPTIMIZATION] recycle old_Head instead of declaring new var
    COORD diff_COORD;                                       // variable indicating distance between 2 pivots

    pivot_Cont.push_front(old_Head);

        // body collision check by "walking" down snake

    QUEUE<COORD>::iterator pivot_It = pivot_Cont.begin();

    for (; pivot_It != --pivot_Cont.end(); pivot_It++){         // notice this only walks until the second-to-last pivot point (hence the --pivot_Cont);
                                                                // this is necessary to account for whether egg was eaten or not.
        prev_pivot = next_pivot;                                // setting new prev_pivot for next iteration
        next_pivot = (*pivot_It);

        diff_COORD.X = prev_pivot.X - next_pivot.X;             // ... calculating delta distance between pivots
        diff_COORD.Y = prev_pivot.Y - next_pivot.Y;


             // (amortized diff_COORD update (might not need to update diff_coord)) + COLLISION TESTING. TEST THIS.

        if (diff_COORD.X > 0){              // case: direction == RIGHT; decrement towards left
            if (Head.Y == prev_pivot.Y){
                if (prev_pivot.X > Head.X && Head.X >= next_pivot.X){
                    return(0);
                }
            }
        }
        else if (diff_COORD.X < 0){        // case: direction == LEFT; decrement towards right
            if (Head.Y == prev_pivot.Y){
                if (prev_pivot.X < Head.X && Head.X <= next_pivot.X){
                    return(0);
                }
            }

        }
        else if (diff_COORD.Y > 0){        // case: direction == DOWN; decrement towards up
            if (Head.X == prev_pivot.X){
                if (prev_pivot.Y > Head.Y && Head.Y >= next_pivot.Y){
                    return(0);
                }
            }
        }
        else if (diff_COORD.Y < 0){        // case: direction == UP; decrement towards down
            if (Head.X == prev_pivot.X){
                if (prev_pivot.Y < Head.Y && Head.Y <= next_pivot.Y){
                    return(0);
                }
            }
        };
    };

            // Final iteration iterating from n-2 to n-1
            // amortized with an egg_eaten check; tail decremented if so.

        prev_pivot = next_pivot;              // setting new prev_pivot for next iteration
        next_pivot = (*pivot_It);

        diff_COORD.X = prev_pivot.X - next_pivot.X;
        diff_COORD.Y = prev_pivot.Y - next_pivot.Y;


        if (diff_COORD.X > 0){              // case: direction == RIGHT; decrement MM towards right
            if (!egg_eaten)
                next_pivot.X++;
            else
                length++;

            if (Head.Y == prev_pivot.Y){
                if (prev_pivot.X > Head.X && Head.X >= next_pivot.X){
                    return(0);
                }
            }
        }

        else if (diff_COORD.X < 0){        // case: direction == LEFT; decrement MM towards left
            if (!egg_eaten)
                next_pivot.X--;

            else
                length++;

            if (Head.Y == prev_pivot.Y){
                if (prev_pivot.X < Head.X && Head.X <= next_pivot.X){
                    return(0);
                }
            }
        }

        else if (diff_COORD.Y > 0){        // case: direction == DOWN; decrement MM towards down
            if (!egg_eaten)
                next_pivot.Y++;
            else
                length++;
            if (Head.X == prev_pivot.X){
                if (prev_pivot.Y > Head.Y && Head.Y >= next_pivot.Y){
                    return(0);
                }
            }
        }

        else if (diff_COORD.Y < 0){        // case: direction == UP; decrement MM towards up
            if (!egg_eaten)
                next_pivot.Y--;
            else
                length++;

            if (Head.X == prev_pivot.X){
                if (prev_pivot.Y < Head.Y && Head.Y <= next_pivot.Y){
                    return(0);
                }
            }
        };



    Tail = next_pivot;                       // Setting the Tail itself

    if (prev_pivot.X == next_pivot.X && prev_pivot.Y == next_pivot.Y)  {          // removing final pivot if it overlaps with second-to-last pivot (intuitively, this hsoudl make sense)
        pivot_Cont.pop_back();
    }

    return(1);
}



COORD Egg::make_new_Egg(COORD& snake_Head, gameBoard& env) {

         // COORD of new egg defined as any coordinate different than original position
         // values here represent CORNERS of enclosure
    std::uniform_int_distribution<SHORT> X_DISTR(env.BOARD_LEFT_BORDER+1, env.BOARD_RIGHT_BORDER-1);      // Declares distribution "template" with lower bound and upper bound.

    //std::random_device y_rd;
    //std::default_random_engine  y_gen(static_cast<long unsigned int>(time(0)));
    std::uniform_int_distribution<SHORT> Y_DISTR(env.BOARD_TOP_BORDER+1, env.BOARD_BOTTOM_BORDER-1);      // Declares distribution "template" with lower bound and upper bound.

    SHORT new_X;
    SHORT new_Y;


    do {                                                                             // Generate new coord under following cases:
        new_X = X_DISTR(env.rand_eng);
        new_Y = Y_DISTR(env.rand_eng);
    }
    while ((new_X == current_Position.X && new_Y == current_Position.Y) ||           // new coord = old egg pos
            (new_X == snake_Head.X && new_Y == snake_Head.Y) );                      // new coord = snake head
                                                                                     // new coord = snake body (tricky to implement)??
    COORD newEgg = {new_X, new_Y};

    return(newEgg);

};

void Egg::update_Egg(COORD& new_Egg) {
    current_Position.X = new_Egg.X;
    current_Position.Y = new_Egg.Y;
};

void Egg::draw_new_Egg(COORD& snake_Head, gameBoard& env) {
    DWORD cCharsWritten;
    COORD newEgg = make_new_Egg(snake_Head, env);
    update_Egg(newEgg);
    FillConsoleOutputCharacter( env.hStdout, (TCHAR)'O', 1, current_Position, &cCharsWritten);
};

DIRECTION parse_Key(DIRECTION DEFAULT) {        // Reads keystroke; up, down, right and left arrow key is converted into its respective direction.
                                                // All other keystrokes and mouse clicks pause the game
    getch();
    char input2 = getch();

    switch(input2){
        case(72):
            return DIRECTION::UP;
        case(80):
            return DIRECTION::DOWN;
        case(75):
            return DIRECTION::LEFT;
        case(77):
            return DIRECTION::RIGHT;
        default:
            return DEFAULT;
    };

};


int main (int argc, char* argv[]) {

                // Main menu "module"

    if (argc == 1) {        // argc = 1 parameters means starting game directly from cmd line

            // Setting Menu parameters

        int flag = INITIALIZE;
        displayMenu();

            // initating option selection "loop" from the menu. Terminate upon calling exitgame.

      while(flag != EXITGAME){                                          // will only terminates when user enters EXITGAME, or terminates from outside (CTRL+C)
         flag = request_User_Input(min_selection,max_selection);        // requests user input <= 20 times before terminating

            // Menu option control switch
        switch(flag){
         case STARTGAME:
         {
          // relaunch snake in a new process window

            STARTUPINFOA si;
            PROCESS_INFORMATION pi;

         // set the size of the structures
           ZeroMemory( &si, sizeof(si) );
           si.cb = sizeof(si);
           ZeroMemory( &pi, sizeof(pi) );
           char ng[] = "Snake.exe start-game";

         // start the program up
          CreateProcessA( "E:\\Programming\\@Side-projects\\Standalone\\Snake\\bin\\Release\\Snake.exe",   // the path
            ng,             // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            CREATE_NEW_CONSOLE, // new console flag
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory
            &si,            // Pointer to STARTUPINFO structure
            &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
            );


          // wait for child process to terminate

            WaitForSingleObject( pi.hProcess, INFINITE );


          // Close process and thread handles.
            CloseHandle( pi.hProcess );
            CloseHandle( pi.hThread );

            break;
          }

          case EXITGAME:                     // Menu selection = 2. Start game
          {
            std::cout << "\n                See ya!            " << std::endl;
            break;
          }
        }
      }
    };



                 // "Actual snake game" module

    if (strcmp(argv[1], "start-game") == 0){         // Start "realSnake" in another second overlapping console window here

            //  Changing second window settings (see snake_Help)
        HWND consoleWindow = GetConsoleWindow();
        DWORD dwStyle = WS_CAPTION | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU;       // "resize" and "maximizebox" flags not set, so window size cannot be changed by user
        SetWindowLong(consoleWindow, GWL_STYLE, dwStyle);                               // sets the styles flagged by dwStyle
        SetWindowPos(consoleWindow, 0, 0, 4, 0, 0, SWP_SHOWWINDOW | SWP_FRAMECHANGED);  // sets window position

            // Initializing board objects
          gameBoard _Board;
          Snake _Snake;
          Egg _Egg;

            // Initializing game parameters

          int terminate = 0;                                            // termination con; 0 = game continues; 1 = game loss; 2 = game win
          DIRECTION recent_dir = DIRECTION::LEFT;

          _Board.initialize_Board_Obj(_Snake, _Egg);
          _Board.print_initial_Board(_Snake, _Egg);

            // game loop

          while (!terminate) {                                           // calls listed events / time tick
            if (kbhit()) {                                               // detects key press
                recent_dir = parse_Key(recent_dir);
            }

            Sleep(100);                                                  // Sleep(100ms) for windows; sleep() / USleep() for Unix
            terminate = _Board.update_Objects(recent_dir, _Snake, _Egg);

          };

        // continues is false; game terminates.

            if (terminate == 1)                                         // terminate == 1 -> loss
                std::cout << "Game over! Try again!" << std::endl;
            else                                                        // terminate == 2 -> winn
                std::cout << "You won! GG!" << std::endl;

            std::cout << "FINAL SCORE:   " << _Board.getScore() << std::endl;

        };


        //! RESERVED. Additional strcmp argv parameters here (for future versions)


        system("pause");
        return(1);

};

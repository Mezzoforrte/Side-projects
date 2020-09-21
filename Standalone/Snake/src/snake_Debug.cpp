/************************ MANUAL *************************

Traditional snake game for the Windows command line.
Use the arrow keys to move.
Gain points and a longer body from eating eggs.
But the snake dies if it touches a wall or its own body!
Win condition? A body as large as the game board itself!

*********************************************************/

/********************* VERSION INFO **********************
??? - Version 1.0. Barebone snake game
      start and exit game menu options
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

BUGS:
[BUGS]
1) @X - configuration still allows for back and forth movement
*********************************************************/

/********************** WHITEBOARD ***********************

Planning considerations:
- Do this in Linx.
- C++ may not be the best way to do this
- How to "clear console screen" per time tick
- How to keep some elements the same, visually, on the console screen, per time step.
- One way to do "updatesnake" method is to "add to head" and "delete last"
  - Requires deque to keep track of coords from head--> tail
  - Alternatively: structure keeping track of every turn, then inc/dec the value / index position
    (visualize, then prove!!)
    --> update: it's DECREMENT "n", and the sum of #s stored in each of the pos = n (or maybe n-1)


TO DO:
- study conio.h kbhit [DONE]
- implement update_Screen [DONE; certain implementation details left resolved for later]
- craft algorithm to "check" for self-collision (terminate() condition) [DONE]
   - Wall termination check [DONE]
- executive decision: identify key "global" parameters, and determine how to store [DONE]

- How to initialize program in new console window? (possibly via a system("cmd.exe "path/to/actual/exe") [DONE]


- Completely finish game_Loop

- Polish and test every other bit of code, starting with updateObjets, and the terminate (in order of priority):
   - Initialize parameters[DONE]
   - Displayboard [DONE]
   - UpdateObjects [DONE]
   - parse_key [DONE]
   - draw_screen [DONE]
   - requestuserinput [DONE]
   - terminate [DONE]

   --- All code compiles, as of 08/16; now for testing phase: --

   1) menu_input_request correct? Mostly.
   2) menu options correct? Yes
   3) Random_egg correct? Yes.
   4) initial gameboard draw correct? Yes.
   5) Actual snake game correcT? TBH.

**********************************************************/


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


// NOTE: Any methods involving "DIRECTION", can also be converted some Windows.h::COORD method.
// Both are used here sake of the author's learning, but efficiency wise, they theoretically should not differ much.

enum class DIRECTION {UP, RIGHT, DOWN, LEFT};            //! enum class used academically but also for clarity



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

        // structors (gameBoard)
    gameBoard();                        // CTOR
    friend Egg;                         //  both Egg and Snake need access to gameBoard member variables
    friend Snake;

    void initialize_Board_Obj(Snake& ini_Snake, Egg& ini_Egg);                                      // initializes board state with egg and snake
    int print_initial_Board(Snake& ini_Snake, Egg& ini_Egg);                                        // displays initial board in console window
    int update_Objects(DIRECTION& new_dir, Snake& snake_PC, Egg& egg_PC);                           // updates object parameters / cycle
    void draw_Screen(Snake snake_PC, Egg egg_PC, COORD old_Head, COORD old_Tail, bool egg_eaten);   // displays board after each cycle
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

class Snake {                                                                       //! COMPLETE.
    int length;
    COORD Head;                                 // represents CURRENT head COORD
    COORD Tail;                                 // represents PREVIOUS tail COORD
    void initialize_Snake(COORD new_COORD);     // initialize_Snake sets head / tail to middle of screen.
                                                // Why this isn't in the CTOR? B/c this makes more "procedural" sense.
public:
        // structors (Snake)
    Snake(): length(1), pivot_Cont(*(new QUEUE<COORD>)) {};

    friend gameBoard;                                                           // gameBoard::initialize_Board_Obj needs access to initialize_Snake; other methods need access to length.
    QUEUE<COORD> pivot_Cont;                                                    // represents "pivot" coordinates in the snake's body. Pivot defined as "corner" of snake or the tail coord
    int update_Head(DIRECTION& new_dir, gameBoard& env);                        // updates position of head. TRUE if head updates successfully.
    int update_Snake(DIRECTION& new_dir, COORD old_Head, bool egg_eaten);       // updates position of snake. TRUE if updates successfully.
    COORD getHead() { return Head; };                                           // returns Head COORD
};

gameBoard::gameBoard() : hStdout(GetStdHandle(STD_OUTPUT_HANDLE)), score(0) {    //! COMPLETE. T&W.

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


           // Debug purposes only! Do not uncomment otherwise!
//           std::cout << "\n\n\n";
//      std::cout << "Current buffer size: (" << Board_p.dwSize.X << ", " << Board_p.dwSize.Y << ")" << std::endl;
//      // std::cout << "Current max window size: (" << max_WindowSize.X << " , " << max_WindowSize.Y << ")" << std::endl;
//      std::cout << "dwMaximumWindowSize: (" << Board_p.dwMaximumWindowSize.X << " , " << Board_p.dwMaximumWindowSize.Y << ")" << std::endl;
//      std::cout << "srWindow (L, R, U, D): (" << Board_p.srWindow.Left << " , " << Board_p.srWindow.Right << " , "
//        << Board_p.srWindow.Top << " , " << Board_p.srWindow.Bottom << std::endl;

          // Obtaining max window size aprameters

      COORD max_WindowSize = {GetLargestConsoleWindowSize(hStdout).X, GetLargestConsoleWindowSize(hStdout).Y};
      _SMALL_RECT MaxWindowSize;            // declaring a RECT to store new window
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

            // Post-set; Debug purposes only! Do not uncomment otherwise!
//
//      std::cout << "Current buffer size: (" << Board_p.dwSize.X << ", " << Board_p.dwSize.Y << ")" << std::endl;
//      // std::cout << "Current max window size: (" << GetLargestConsoleWindowSize(hStdout).X << " , " << max_WindowSize.Y << ")" << std::endl;
//      std::cout << "dwMaximumWindowSize: (" << Board_p.dwMaximumWindowSize.X << " , " << Board_p.dwMaximumWindowSize.Y << ")" << std::endl;
//      std::cout << "srWindow (L, R, U, D): (" << Board_p.srWindow.Left << " , " << Board_p.srWindow.Right << " , "
//        << Board_p.srWindow.Top << " , " << Board_p.srWindow.Bottom << std::endl;

            // Setting gameboard borders based on csbi info (with assert, if possible)

        BOARD_TOP_BORDER = 2;                                      // "2" instead of "0" to allow for "heads up display"
        BOARD_LEFT_BORDER = 0;
        BOARD_BOTTOM_BORDER = Board_p.srWindow.Bottom;
        BOARD_RIGHT_BORDER = Board_p.srWindow.Right;

}

int gameBoard::print_initial_Board(Snake& ini_Snake, Egg& ini_Egg) {                //! COMPLETE. T&C.

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

int gameBoard::update_Objects(DIRECTION& new_dir, Snake& snake_PC, Egg& egg_PC) {   //! COMPLETE. Updates position of snake and egg per time tick (does not change visuals)   //! INCOMPLETE.

    COORD oldHead = snake_PC.Head;
    COORD oldTail = snake_PC.Tail;

        //! DEBUG
        //  std::cout << "oldhead: " << oldHead.X << ",   " << oldHead.Y << "   " << "oldTail:  " << oldTail.X << "   " << oldTail.Y;

    bool egg_eaten = (snake_PC.Head.X == egg_PC.current_Position.X && snake_PC.Head.Y == egg_PC.current_Position.Y);    //tested; works.


        //! DEBUGGING ONLY - debugging code works with just 1 head + 1 tail coordinate
        //  std::cout << oldHead.X << "," << oldHead.Y << "    " << oldTail.X << "," << oldTail.Y << "      ";
        //  snake_PC.Tail = snake_PC.Head;


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

void gameBoard::draw_Screen(Snake snake_PC, Egg egg_PC, COORD old_Head, COORD old_Tail, bool egg_eaten) {    //! COMPLETE & CORRECT. BUT reduce args ifposs.

//    std::cout << "Head at: " << snake_PC.Head.X << ", " << snake_PC.Head.Y << ".  Tail at: " << snake_PC.Tail.X << ", " << snake_PC.Tail.Y <<
//     ". X at: " << old_Head.X << ", " << old_Head.Y << ". Clear at: " << old_Tail.X << ", " << old_Head.Y << ". " << '\r';
        //!UPDATE: HEAD NO LONGER DUPLICATES ITSELF, BUT STILL WAYS TO GO; NECK SEEMS TO ITERATE ENDLESSLY NOW.

    DWORD cCharsWritten;

        // Redrawing Snake body:

    if (!egg_eaten) {               //! TESTED; WORKS.
            // Clear old tail

        //! DEBUG
        //  std::cout << "   DEBUG: tailclear  ";
        if (!(old_Tail.X == egg_PC.current_Position.X && old_Tail.Y == egg_PC.current_Position.Y))         // for minor case where egg egg spawns on body, and tail is about to clear it.
            FillConsoleOutputCharacter( hStdout, (TCHAR)' ', 1, old_Tail, &cCharsWritten);

        // std::cout << " (DB) clearing " << old_Tail.X << " , " << old_Tail.Y;

        if (snake_PC.length > 1)
            FillConsoleOutputCharacter( hStdout, (TCHAR)'X', 1, old_Head, &cCharsWritten );  // draws new neck ONLY if length > 1
    }

    else {  // egg is eaten

            // Draw new egg
        FillConsoleOutputCharacter( hStdout, (TCHAR)'0', 1, egg_PC.current_Position, &cCharsWritten );

            // Draw new neck
        FillConsoleOutputCharacter( hStdout, (TCHAR)'X', 1, old_Head, &cCharsWritten );
    };


            // Print new head.
            // there is a great reason why we print the head AFTER the tail update; because
            // POTENTIALLY, if head overlaps new tail segmnet, we want the HEAD to be displayed at that coord.
        FillConsoleOutputCharacter( hStdout, (TCHAR)'@', 1, snake_PC.Head, &cCharsWritten );

};

void Snake::initialize_Snake(COORD new_Snake) {                                     //! COMPLETE.
    Head = new_Snake;
    Tail = new_Snake;
};

void gameBoard::initialize_Board_Obj(Snake& ini_Snake, Egg& ini_Egg) {
    ini_Snake.initialize_Snake({Board_p.dwMaximumWindowSize.X/2 , Board_p.dwMaximumWindowSize.Y/2});        // snake head initialized at center of board
    ini_Egg.make_new_Egg(ini_Snake.Head, *this);
}

int Snake::update_Head(DIRECTION& new_dir, gameBoard& env) {

        std::cout << "Debugging: " << static_cast<int>(new_dir) << ".  ";

                  // on the topic of updating old_Tail (for pedagogical purposes)

            // Do we need an old_tail parameter? Size vs. time complexity tradeoff is MASSIVE.
            // Without Snake::old_Tail, 2 possible methods to calc old_Tail might look like this:

        /*      Module to extract tail from Head COORD (PSEUDO-CODE)

        COORD old_Tail.X = preupdated_HEAD.X;
        COORD old_Tail.Y = preupdated_HEAD.Y;

        for (???) {                                 // walk through each dir_count element from head --> last
            if (dir_count[i].first == DIRECTION::UP)
                tail_COORD.Y += dir_count[i].second;
            else if (last_dir.first == DIRECTION::DOWN)
                tail_COORD.Y -= dir_count[i].second;
            else if (last_dir.first == DIRECTION::RIGHT)
                tail_COORD.X += dir_count[i].second;
            else if (last_dir.first == DIRECTION::LEFT)
                tail_COORD.X -= dir_count[i].second;
        };*/

        /*      Module to extract tail from dir_cont (Possibly more optimized version):

        pair<DIRECTION, int> last_dir = dir_count[dir_count.size());

        if (last_dir.first == DIRECTION::UP)
            old_Tail.Y += 1;
        else if (last_dir.first == DIRECTION::DOWN)
            old_Tail.Y -= 1;
        else if (last_dir.first == DIRECTION::RIGHT)
            old_Tail += 1;
        else if (last_dir.first == DIRECTION::LEFT)
            old_Tail -= 1;
    };
*/

            // push (new_dir, length) to the front if new_dir diff. from most recent dir.
            // Otherwise, do nothing (remember: through iterations, we ONLY change dir_count[i], i >= 1.

//    if ( static_cast<int>(new_Lead) == ((static_cast<int>(dir_count.front().first) + 2)%4) && length > 1)        // checks if bumps into neck segment.
//        return(0);          //! TESTED. WORKS. ... but might not even need anymore...


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

        // declaring paramters needed for each pivot-to-pivot update

    COORD prev_pivot;                                       // will be initialized to 'Head' in the main loop below; redundant to set twice
    COORD next_pivot = Head;                                // initialized to Head but 1st iteration will set it to 1st pivot AFTER Head
                                                            // [OPTIMIZATION] recycle old_Head instead of declaring new var
    COORD diff_COORD;
                                         // variable indicating distance between 2 pivots
//    - old code -
//    if (length == 1){                                   // length == 1 is very special case; requires special condition
//        Tail = Head;
//
//    }
//    else if (new_dir != prev_Lead) {                    // "else=if" critical in cases where we change direction when length == 1
//        pivot_Cont.push_front(old_Head);                //! TEST THIS: check case for neck collision....
//        // prev_Lead = new_dir;                            //! CHECK CASE FOR PREV_LEAD CHANGE.
//    };

    pivot_Cont.push_front(old_Head);                        //! this POS works, but stops collission check from working ....

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
                    std::cout << " Updatesnake Failed: " << std::endl;
                    std::cout << "prev: " << prev_pivot.X << ", " << prev_pivot.Y << "   . Head: " << Head.X << ", " << Head.Y <<
                    "    . NP: " << next_pivot.X << ", " << next_pivot.Y << "    . diffCOORD: " << diff_COORD.X << ", " << diff_COORD.Y << std::endl;

                    return(0);
                }
            }
        }
        else if (diff_COORD.X < 0){        // case: direction == LEFT; decrement towards right
            if (Head.Y == prev_pivot.Y){
                if (prev_pivot.X < Head.X && Head.X <= next_pivot.X){
                    std::cout << " Updatesnake Failed: " << std::endl;
                    std::cout << "prev: " << prev_pivot.X << ", " << prev_pivot.Y << "   . Head: " << Head.X << ", " << Head.Y <<
                    "    . NP: " << next_pivot.X << ", " << next_pivot.Y << "    . diffCOORD: " << diff_COORD.X << ", " << diff_COORD.Y << std::endl;

                    return(0);
                }
            }

        }
        else if (diff_COORD.Y > 0){        // case: direction == DOWN; decrement towards up
            if (Head.X == prev_pivot.X){
                if (prev_pivot.Y > Head.Y && Head.Y >= next_pivot.Y){
                    std::cout << " Updatesnake Failed: " << std::endl;
                    std::cout << "prev: " << prev_pivot.X << ", " << prev_pivot.Y << "   . Head: " << Head.X << ", " << Head.Y <<
                    "    . NP: " << next_pivot.X << ", " << next_pivot.Y << "    . diffCOORD: " << diff_COORD.X << ", " << diff_COORD.Y << std::endl;

                    return(0);
                }
            }
        }
        else if (diff_COORD.Y < 0){        // case: direction == UP; decrement towards down
            if (Head.X == prev_pivot.X){
                if (prev_pivot.Y < Head.Y && Head.Y <= next_pivot.Y){
                    std::cout << " Updatesnake Failed: " << std::endl;
                    std::cout << "prev: " << prev_pivot.X << ", " << prev_pivot.Y << "   . Head: " << Head.X << ", " << Head.Y <<
                    "    . NP: " << next_pivot.X << ", " << next_pivot.Y << "    . diffCOORD: " << diff_COORD.X << ", " << diff_COORD.Y << std::endl;

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

        // std::cout << "   COORD == CHECK:    " << (prev_pivot.Y == next_pivot.Y) << "  " <<  std::endl;

        if (diff_COORD.X > 0){              // case: direction == RIGHT; decrement MM towards right
            if (!egg_eaten)
                next_pivot.X++;
            else
                length++;

            if (Head.Y == prev_pivot.Y){
                if (prev_pivot.X > Head.X && Head.X >= next_pivot.X){
                    std::cout << " Updatesnake Failed: " << std::endl;
                    std::cout << "prev: " << prev_pivot.X << ", " << prev_pivot.Y << "   . Head: " << Head.X << ", " << Head.Y <<
                    "    . NP: " << next_pivot.X << ", " << next_pivot.Y << "    . diffCOORD: " << diff_COORD.X << ", " << diff_COORD.Y << std::endl;


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
                    std::cout << " Updatesnake Failed: " << std::endl;
                    std::cout << "prev: " << prev_pivot.X << ", " << prev_pivot.Y << "   . Head: " << Head.X << ", " << Head.Y <<
                    "    . NP: " << next_pivot.X << ", " << next_pivot.Y << "    . diffCOORD: " << diff_COORD.X << ", " << diff_COORD.Y << std::endl;
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
                    std::cout << " Updatesnake Failed: " << std::endl;
                    std::cout << "prev: " << prev_pivot.X << ", " << prev_pivot.Y << "   . Head: " << Head.X << ", " << Head.Y <<
                    "    . NP: " << next_pivot.X << ", " << next_pivot.Y << "    . diffCOORD: " << diff_COORD.X << ", " << diff_COORD.Y << std::endl;


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
                    std::cout << " Updatesnake Failed: " << std::endl;
                    std::cout << "prev: " << prev_pivot.X << ", " << prev_pivot.Y << "   . Head: " << Head.X << ", " << Head.Y <<
                    "    . NP: " << next_pivot.X << ", " << next_pivot.Y << "    . diffCOORD: " << diff_COORD.X << ", " << diff_COORD.Y << std::endl;


                    return(0);
                }
            }
        };



    Tail = next_pivot;                       // Setting the Tail itself

            std::cout << "prev: " << prev_pivot.X << ", " << prev_pivot.Y << "    . NP: " << next_pivot.X << ", " << next_pivot.Y <<
        "   . Head: " << Head.X << ", " << Head.Y << "    . diffCOORD: " << diff_COORD.X << ", " << diff_COORD.Y << "              " << '\r';

    if (prev_pivot.X == next_pivot.X && prev_pivot.Y == next_pivot.Y)  {          // removing final pivot if it overlaps with second-to-last pivot (intuitively, this hsoudl make sense)
        pivot_Cont.pop_back();
    }

    return(1);
}



COORD Egg::make_new_Egg(COORD& snake_Head, gameBoard& env) {        //! COMPLETE (avg); IC: snake body fringe case.

            // COORD of new egg defined as any coordinate different than original position

            // values here represent CORNERS of enclosure
    std::uniform_int_distribution<SHORT> X_DISTR(env.BOARD_LEFT_BORDER+1, env.BOARD_RIGHT_BORDER-1);      // Declares distribution "template" with lower bound and upper bound.

    //std::random_device y_rd;
    //std::default_random_engine  y_gen(static_cast<long unsigned int>(time(0)));
    std::uniform_int_distribution<SHORT> Y_DISTR(env.BOARD_TOP_BORDER+1, env.BOARD_BOTTOM_BORDER-1);      // Declares distribution "template" with lower bound and upper bound.

    SHORT new_X;
    SHORT new_Y;


                                                                                    // Generate new coord while the following invalid cases occur
    do {                                                                            //! BUG: potential infinite loop
        new_X = static_cast<SHORT>(X_DISTR(env.rand_eng));
        new_Y = static_cast<SHORT>(Y_DISTR(env.rand_eng));
    }
    while ((new_X == current_Position.X && new_Y == current_Position.Y) ||           // new coord = old egg pos
            (new_X == snake_Head.X && new_Y == snake_Head.Y) );                      // new coord = snake head
                                                                                     // new coord = snake body (tricky to implement)??
    COORD newEgg = {new_X, new_Y};

    return(newEgg);

};

void Egg::update_Egg(COORD& new_Egg) {                              //! COMPLETE.
    current_Position = new_Egg;
};

void Egg::draw_new_Egg(COORD& snake_Head, gameBoard& env) {
    DWORD cCharsWritten;
    COORD newEgg = make_new_Egg(snake_Head, env);
    update_Egg(newEgg);
    FillConsoleOutputCharacter( env.hStdout, (TCHAR)'O', 1, current_Position, &cCharsWritten);
};

DIRECTION parse_Key(DIRECTION DEFAULT) {     //! COMPLETE for average case (Bug: Currently pauses when pressing any other key / mouse click)
    getch();
    char input2 = getch();

    switch(input2){              // basic cases for now; there ARE states where key is HELD rather than pressed
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
                            //! BUG (or feature): typing "snake.exe start-game" on cmd line will automatically start the game

            // Setting Menu parameters

        int flag = INITIALIZE;
        displayMenu();

            // initiating loop flow from the menu. Terminate upon calling exitgame.

      while(flag != EXITGAME){                                          // will only terminates when user enters EXITGAME, or terminates from outside (CTRL+C)
         flag = request_User_Input(min_selection,max_selection);        // requests user input <= 20 times before terminating

            // Menu option control switch
        switch(flag){
         case STARTGAME:                                                // Menu selection = 1. Start game
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
          CreateProcessA( "E:\\Programming\\@Side-projects\\Standalone\\Snake\\bin\\Debug\\snake_Debug.exe",   // the path
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

    if (strcmp(argv[1], "start-game") == 0){         // Start "realSnake" in another console here
           //  Changing second window settings (see snake_Help)
        HWND consoleWindow = GetConsoleWindow();
        DWORD dwStyle = WS_CAPTION | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU;             // "resize" and "maximizebox" flags not set, so window size cannot be changed by user
//      DWORD style = GetWindowLong(consoleWindow, GWL_STYLE);
        SetWindowLong(consoleWindow, GWL_STYLE, dwStyle);                                     // sets the styles flagged by dwStyle
        SetWindowPos(consoleWindow, 0, 0, 4, 0, 0, SWP_SHOWWINDOW | SWP_FRAMECHANGED);        // sets window position; see windows documentation

            // Initializing board objects
          gameBoard _Board;
          Snake _Snake;
          Egg _Egg;

            // Initializing game parameters

          int terminate = 0;                                            // termination con; 0 = game continues; 1 = game loss; 2 = game win
          DIRECTION recent_dir = DIRECTION::LEFT;                       // setting initial direction to LEFT

          _Board.initialize_Board_Obj(_Snake, _Egg);
          _Board.print_initial_Board(_Snake, _Egg);

            // game loop

          while (!terminate) {                                          // calls listed events / time tick
            if (kbhit()) {                                              // detects key press
                recent_dir = parse_Key(recent_dir);  //!!! THIS MAY NOT BE MOST OPTIMAL WAY. SIT ON THIS
                //alternatively, DIRECTION new_dir = parse_Key, then pass new_dir to update_Key --> more logical, but mcuh less efficient.
                // Update: after deliberation with rest of methods, come to conclusion that recent_Dir variable needed, BUT CANNOT
                //         replace recent_Dir of the most recently added direction to snake's container. Thus, structure now has:
                //         DIRECTION recent_Dir AND snake's dir_count must store data on EVERY node (previously, overlapped the head.dir
                //          with recent_Dir, which made things messy)

            }

            Sleep(100);                                                  // Sleep(sec) for windows; sleep() / USleep() for Unix
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


    // MORE ROUGH WORK.

///*   ALGORITHM (OBJECTIFIED)
// *     0) display(Loading menu)
// *     1) display(setBoard)
// *     2) run SNAKEmodule:
// *         while(terminate == false) {
// *             run_sequence;
// *             sequence:
// *                 1) key_input_pause //! (0.25s)
// *                 2) update_screen;  //! if no input detected, pause additional (0.25s) to "compensate" for additional actions;
// *             check_terminate();
// *         };
// */
//
//
//            // SNAKE "MODULE"
//
//        // Object declarations
//    Snake snake_PC;
//    terminate = false;
//
//
//
///*              ORIGINAL GAME PROGRAM. SOME SYNTAXES WORTH CHECKING OUT.
//    bool isrunning = true;
//    std::string motion = "X";
//    int whitecount;
//
//    while (isrunning){
//
//        system("cls");
//        // Instead of running cls, can keep rest of screen teh same and use
//        // setcursorposition to set the cursor to the head of the snake, then clearn everything
//        // after cursor, then reprinting new diagram. This is still fuckign shitty.
//
//
//        std::cout << "***************" << std::endl;
//        std::cout << motion << std::endl;
//
//        system("pause>nul");
//
//        if(GetAsyncKeyState(VK_RIGHT))
//            whitecount++;
//        if (GetAsyncKeyState(VK_LEFT))
//            whitecount--;
//        if (GetAsyncKeyState(VK_UP))
//            whitecount--;
//
//
//        if (whitecount > 15)
//            whitecount = 15;
//
//        else if (whitecount < 0)
//            whitecount = 0;
//
//        motion = "X";
//
//        for (int i = 0; i < whitecount; i++) {
//            motion = " " + motion;
//        };
//    };
//        system("pause");
//*/
//
//
//
//




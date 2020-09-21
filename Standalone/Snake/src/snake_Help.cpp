/***********************************
Snake help:

    1) how to create new Window when running an exe (system(whatever))
    2) how setwindowparameters work
    3) demo of how to create the initial gameBoard
    4) khbit demo
    5) asynckeystate demo
************************************/

#define _WIN32_WINNT 0x0601


#include <conio.h>
#include <windows.h>
#include <iostream>
#include <cstring>
#include <cassert>
#include <memory>
#include <random>
#include <chrono>
#include "mingw.thread.h"                   // for thread.sleep
#include <list>
#include "../lib/Common/Common.h"



//std::string videotape::exception_test() {                   // Test if a string contains any valid integers using stoi. WORKS NORMAL CASE. TO-TEST:
//    std::string input;                                      // HOW MANY TIMES CAN YOU RUN EXCEPTION TEST (Limit)? Test weird inputs.
//    std::cin >> input;
//    try {
//        std::stoi(input);
//    } catch (...) {
//        std::cout << "Your input is: " << input << " ... which is incorrect. Please try again: ";
//        input = exception_test();
//    };
//    return (input);
//};


int randnum(auto rand_eng){
    std::uniform_int_distribution<int> dist(0, 10);
    return dist(rand_eng);
}


COORD getCursorPosition(CONSOLE_SCREEN_BUFFER_INFO scr){
    COORD cursor;

};




class Obj{
public:
    int a;
};



class random_eng{
public:
    std::default_random_engine x_gen;
    random_eng();
};


random_eng::random_eng(): x_gen((*(new std::random_device()))()) {

}

enum class enumerator {one, two, three};


int main (int argc, char* argv[]) {

            // RANDOM DEVICE AND ENGINE TEST
        random_eng a;
        std::default_random_engine x_gen;

        std::uniform_int_distribution<std::mt19937::result_type> dist(1,10);
        std::random_device rd;
        unsigned seed;

        if (0 != rd.entropy())
        {
           // if random_device works, then we set seed accordingly.
           seed = rd();
        }
        else
        {
           // otherwise: no random_device available, seed using the system clock
           seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
        }


        x_gen.seed(seed);


        std::cout << dist(a.x_gen) << std::endl;
        std::cout << randnum(a.x_gen) << std::endl;



    float SELECTION = 9.0;

    if (SELECTION == 1.0 && argc > 1 && std::strcmp(argv[1], "newgame") == 0){
        // if (std::strcmp(argv[1], "newgame") == 0) {      // demos new window running exe
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

   // set the size of the structures
       ZeroMemory( &si, sizeof(si) );
       si.cb = sizeof(si);
       ZeroMemory( &pi, sizeof(pi) );
      char ng[] = "tester2.exe newgame";

  // start the program up
      CreateProcessA( "E:\\Programming\\@Side-projects\\Standalone\\tester2.exe",   // the path
        ng,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE, // new console flag
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
        );
        // Close process and thread handles.
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );

        system("pause");
        // };
    }

    else if (SELECTION == 2.0) {            // Demonstrates Screen Buffer parameters


    // screen parameters
      DWORD cCharsWritten;
      HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);         // extracting "handle".
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      GetConsoleScreenBufferInfo(hStdout, &csbi);               // extract screenbuffer info.

      COORD max_WindowSize = {GetLargestConsoleWindowSize(hStdout).X , GetLargestConsoleWindowSize(hStdout).Y};

            //      typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
            //        COORD dwSize;                     // current buffer size; literally can be infinity buffer
            //        COORD dwCursorPosition;
            //        WORD wAttributes;
            //        SMALL_RECT srWindow;              // "default" starting window coords
            //        COORD dwMaximumWindowSize;        // "largest DISPLAYABLE console size", given hstdout AND buffer size.
            //      } CONSOLE_SCREEN_BUFFER_INFO;

            //      COORD MaxConsoleWindowSize;         // coord not stored in CSBI; determines largest console window possible, given
                                                        // stdout (hardware) specs

      std::cout << "Current buffer size: (" << csbi.dwSize.X << ", " << csbi.dwSize.Y << ")" << std::endl;
      std::cout << "Current max window size: (" << max_WindowSize.X << " , " << max_WindowSize.Y << ")" << std::endl;
      std::cout << "dwMaximumWindowSize: (" << csbi.dwMaximumWindowSize.X << " , " << csbi.dwMaximumWindowSize.Y << ")" << std::endl;
      std::cout << "srWindow (L, R, U, D): (" << csbi.srWindow.Left << " , " << csbi.srWindow.Right << " , "
        << csbi.srWindow.Top << " , " << csbi.srWindow.Bottom << std::endl;

        // Showing difference between dwmaxwindowsize and...
    FillConsoleOutputCharacter( hStdout,                   // Handle to console screen buffer
                                (TCHAR)'O',                // Character to write to the buffer
                                 1,                        // Number of cells to write
                                 {csbi.dwMaximumWindowSize.X - 1, csbi.dwMaximumWindowSize.Y},    // Coordinates of first cell
                                 &cCharsWritten );         //


        // srWindow size
    FillConsoleOutputCharacter( hStdout,                   // Handle to console screen buffer
                                (TCHAR)'X',                // Character to write to the buffer
                                 1,                        // Number of cells to write
                                 {csbi.srWindow.Right, csbi.srWindow.Bottom},    // Coordinates of first cell
                                 &cCharsWritten );


      _SMALL_RECT MaxWindowSize;            // declaring a RECT to store new window
          MaxWindowSize.Left = 0;
          MaxWindowSize.Top = 0;
          MaxWindowSize.Right = max_WindowSize.X-1;
          MaxWindowSize.Bottom = max_WindowSize.Y-1;


            // Resetting windows buffer, followed by resetting console windows size

      std::cout << "\n   setbuffer: " << SetConsoleScreenBufferSize(hStdout, max_WindowSize);
      std::cout << ".     setwindow: " << SetConsoleWindowInfo(hStdout, 1, &MaxWindowSize);
      GetConsoleScreenBufferInfo(hStdout, &csbi);               // extract screenbuffer info.

      std::cout << ".   After setting: \n\n";
      std::cout << "Current buffer size: (" << csbi.dwSize.X << ", " << csbi.dwSize.Y << ")" << std::endl;
      std::cout << "Current max window size: (" << max_WindowSize.X << " , " << max_WindowSize.Y << ")" << std::endl;
      std::cout << "dwMaximumWindowSize: (" << csbi.dwMaximumWindowSize.X << " , " << csbi.dwMaximumWindowSize.Y << ")" << std::endl;
      std::cout << "srWindow (L, R, U, D): (" << csbi.srWindow.Left << " , " << csbi.srWindow.Right << " , "
        << csbi.srWindow.Top << " , " << csbi.srWindow.Bottom << std::endl;

            // showing behaviour of fillconsoeloutputcharacter from start to end coord. Result: it wraps around.
    FillConsoleOutputCharacter( hStdout, (TCHAR)'#', 400, {0,0}, 0);


    }

     else if (SELECTION == 3.0) {
        std::cout << request_User_Input(1, 5) << std::endl;
        std::cout << request_User_Input(35, 78) << std::endl;

     }


    else if (SELECTION == 4.0) {
                // EMPTY.

    }


    else if (SELECTION == 5.0) {                // Enum class and static_cast
        enumerator a = enumerator::two;
        int x = 3;

        auto b = (float)x;
        auto c = static_cast<float>(x);         // static_cast checked at compile time.
        std::cout << b << std::endl;
        std::cout << c << std::endl;
    }


    else if (SELECTION == 6.0) {                // Enum class and static_cast

    }

    else if (SELECTION == 7.0) {                // Enum class and static_cast
        std::list<int> container(10, 2457);
        typename std::list<int>::iterator x = container.begin();
        typename std::list<int>::iterator y = --container.begin();
        // y++;

        std::cout << *x << "  " << *y << std::endl;
        std::cout << (x == container.end()) << std::endl;

    }

    else if (SELECTION == 8.0) {                // how to set console window parameters
        HWND consoleWindow = GetConsoleWindow();                                        // retrieve data structure of window parameter
        DWORD dwStyle = WS_CAPTION | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU;       // CAPTION (title bar); OVERLAPPED (goes on top prev window); MINIMIZEBOX(minimize key); SYSMENU(system menu icon)
        SetWindowLong(consoleWindow, GWL_STYLE, dwStyle);                               // sets the styles
        SetWindowPos(consoleWindow, 0, 0, 4, 0, 0, SWP_SHOWWINDOW | SWP_FRAMECHANGED);  // sets window position

    }

    else if (SELECTION == 9.0) {
        for (int i = 1; i <= 5; ++i)
        {
          std::cout << i << " ";
          std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << std::endl;
    }

    else {
        std::cout << "oldgame" << std::endl;
    };

    system("pause");

}

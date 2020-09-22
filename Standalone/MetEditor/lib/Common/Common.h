/***** CHANGELOG *********
03/?? - Completed first version with printArray accepting a pointer and a reference array, and sizeArray().
03/17 - Added random method
04/04 - Added matMult method (untested); compiles with no probs to typing.
04/05 - Finished MatMult method, and tested for functionality.

*************************/

#include <fstream>
#include <iostream>
#include <string>
#include <random>


#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED


template <class T, size_t n>                        // print array; T(&a)[n] preferable in int main()
void printArray(T (&array)[n], std::ostream& out = std::cout, const int& whitespace = -1)   {
    std::string append = "";                        // Considerations: what about cases where it bleeds off page?
    if (whitespace == -1)                           // Consider adding automatic / manual newline
        append = "\n";
    else {
        for (int i = 0; i < whitespace; i++)
        append = append + " ";
    }

    for (int i = 0; i < n; i++) {
        out << "[" << i << "]: " << array[i] << append;
    }
};

template <class T>                                              // print array; T* preferable in class member functions.
void printArray(T* array, const int arraySize, std::ostream& out = std::cout, const int& whitespace = -1)   {
    std::string append = "";

    if (whitespace == -1)
        append = append + "\n";
    else {
        for (int i = 0; i < whitespace; i++)
        append = append + " ";
    }

    for (int i = 0; i < arraySize; i++)
        out << array[i] << append;
};

template <typename T, size_t n>                                 // returns array size
int sizeArray(T (&array)[n]){
    return (n);
}

int random(int upper, int lower){                               // returns int from range lower - upper.
    std::default_random_engine generator;                       // randomizer "function".
    std::uniform_int_distribution<int> distribution(upper, lower);       // Declares distribution "template" with lower bound and upper bound.
                                                                // Reminder to devs: the seed is "remembered" in between compilations.
                                                                // This means compiler generate won't bother generating new seed after each build.
    int random = distribution(generator);
    return (random);
};

template <class T, int r1, int c1_r2, int c2>
T** matMult(T (&arr1)[r1][c1_r2], T (&arr2)[c1_r2][c2]) {       // Accepts two matrices, with column1 == row 2.

    T** temp = 0;
    temp = new T*[r1];
    for (int i = 0; i < r1; i++)
      temp[i] = new T[c2];

    for (int i = 0; i < r1; i++) {              // i and j loops determine position of summation.
      for (int j = 0; j < c2; j++) {
        T sum = 0;
	    for (int s = 0; s < c1_r2; s++) {       // Logic should be obvious if one knows how to sum up each value in matrix. mult.
              sum = sum + arr1[i][s]*arr2[s][j];  // [i][0 ... c1/r2] * [0 ... c1/r2][j];
        }
        temp[i][j] = sum;
      }
    }

    return (temp);
}

template <class T>
int numDigits(T number) {           // Counts the number of digits in number (supplementary to request_User_Input)
    int digits = 0;

        // what to do if number < 1; default case is to pretend it has 1 digit
    if (number < 0) digits = 1;

    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}


//int request_User_Input(int min_val, int max_val) {    // requests User input at console window; invalidates any non-integer input that is min_val <= input <= int max_val
//
//    std::string first = "";
//    int length = 0;
//    int processed_sel;
//    int out_of_range = 1;
//    COORD initial_cursor;
//
//            // module for obtaining initial cursor position.
//            // CSBI will be called again for new gameBoard.
//
//        DWORD cCharsWritten;
//        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
//        CONSOLE_SCREEN_BUFFER_INFO csbi;
//
//        if(GetConsoleScreenBufferInfo(hStdout, &csbi)) {
//            initial_cursor.X = csbi.dwCursorPosition.X;
//            initial_cursor.Y = csbi.dwCursorPosition.Y;
//        }
//        else {
//            initial_cursor.X = 0;
//            initial_cursor.Y = 0;
//        }
//
//
//    while (out_of_range){
//        assert(++out_of_range <= 21);        // terminates program if user fails to enter valid input 20 times. 20 is arbitrary.
//
//        //! Alternative method to read in lines; somewhat bugged, but does not automatically "force" \n after hitting enter.
//            //        first = "";
//            //        length = 0;
//            //        do {
//            //          std::cout << first << std::endl;
//            //            first += getch();                         //! Need way to handle poor characters. Solution may just be getline(string);
//            //            putchar(first[length++]);
//            //        } while (first[length] != '\r');     // '\r' is char for enter
//
//        std::getline(std::cin, first);
//        length = first.length();
//
//        FillConsoleOutputCharacter(hStdout, ' ', length, initial_cursor, &cCharsWritten);     // "clears" user input from stdout by adding
//                                                                                              // ' ' starting from initial cursor pos 'length' times
//        SetConsoleCursorPosition(hStdout, initial_cursor);                                    // necessary since FillConsoleOutputCharacter does NOT reset cursor
//
//
//            // error handling for std::stoi (which converts the string 'first' into int)
//            // stoi()suboptimal for now. other possibilities: stringstream, std::atoi, homemade (I already have something written up for this...)
//
//        try {
//            processed_sel = std::stoi(first);
//            if(numDigits(processed_sel) == length){             // makes sure the entire string was actually an integer (i.e. stoi("3a") = 3, but isn't valid.)
//                if (min_val <= processed_sel && processed_sel <= max_val)
//                out_of_range = false;
//            };
//        } catch (...) { };
//
//    };
//
//    return(processed_sel);
//
//}


bool swap() { return (1); };

int array(int upper, int lower, int n) { return (0); };        // returns array with n elements in between lower - upper, all of them different.

#endif  // COMMON_H_INCLUDED

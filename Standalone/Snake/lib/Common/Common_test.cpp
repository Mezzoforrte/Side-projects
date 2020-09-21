/**** TEST RESULTS ****
TEST 2a) MatMult() between 2x3 and 3x2 array.
   Expected output:
0  7  14  7  21
0  7  14  7  21
0  7  14  7  21
0  7  14  7  21
0  7  14  7  21

   Actual output:
    See common_test.txt.

   Discussion: MatMult works as intended.

TEST 2b) Non fully initialized matrix. ex (arr1[2][3] = {{0, 1, 1}}} and arr2[3][2] = {{4, 3}, {2, 2}})
   Expected output:
    4  4
    7  6

   Actual output:
    Program hangs and terminates without error or displaying desired array.


*************************/

#include <iostream>
#include <fstream>
#include "Common.h"

using namespace std;

int main(){
    ofstream out("common_test.txt");

    out << "\nTEST 2a): MatMult() between 2x3 and 3x2 array" << "\n\n";
    int arr1[5][5] = {{0, 1, 2, 1, 3}, {0, 1, 2, 1, 3}, {0, 1, 2, 1, 3}, {0, 1, 2, 1, 3}, {0, 1, 2, 1, 3}};
    int arr2[5][5] = {{0, 1, 2, 1, 3}, {0, 1, 2, 1, 3}, {0, 1, 2, 1, 3}, {0, 1, 2, 1, 3}, {0, 1, 2, 1, 3}};

    int** test2a;

    test2a = matMult(arr1, arr2);

    for (int i = 0; i < 5; i++) {
        printArray<int>((test2a)[i], 5, out, 2);
        out << std::endl;
    }

    delete test2a;
    /*** TEST 2B hangs
    out << "\nTEST 2b: MatMult() between not fully initialized array" << "\n\n";
    int arr3[2][3] = {{0, 1, 1}};
    int arr4[3][2] = {{4, 3}, {2, 2}};

    int** test2b;

    test2b = matMult(arr3, arr4);

    for (int i = 0; i < 2; i++) {
        printArray<int>((test2b)[i], 2, out, 2);
        out << std::endl;
    }
    *******/
}

#include <iostream>
#include <vector>
#include <dirent.h>
#include <map>
#include <cstring>
#include <memory>
#include <filesystem>
#include <regex>
#include <utility>

#include "../lib/Common/Common.h"
#include "../lib/batchdock.h"


bool isNumber( std::string token )                          // is positiveinteger?
{
    return std::regex_match( token, std::regex(("[0-9]+")));
}


char* validtokenbeforeEqual(char* prior, char* argument){   //! VERY GENERAL SOLUTION; checks if token before "=" is as expected, and returns the after token.
                                                            //! pseudotested (cpp.sh)
//    char firsttoken[strlen(prior)];
    char totaltoken[strlen(argument)];

    std::strcpy(totaltoken, argument);

    if(!strcmp(prior, std::strtok(totaltoken, "="))){
        return (std::strtok(NULL, ""));
    }
    else
        return (nullptr);

}


std::string regparser(std::vector<std::string>::iterator target, char* first, char* second, char* third, char* fourth, char* fifth){      // this reads the "regexp" then edits name according to the expression
     // first = action, second = flags, third = regexp, fourth = pos, fifth = nochars / iterations
                                                    // SUPER NAIVE: just append index, mostly for testing. [DONE]
                                                    // NAIVE: parses with "semi-walkthrough", i.e. "Which characters (position) do you want to keep? Which ones
                                                    // do you want to delete? " ... then takes a sequence or range of integers
                                                    // HARD: parses expression based on "standard" regular expression rules

            // Reserve for Regular expression function on "second"


            // Converts fourth and fifth into integers (default: position=XX, nochars=XX)


   // testcase:
   // std::string x = regparser(strings.begin(), "numberall", nullptr, nullptr, "position=0", "iterations=1");      // test code for regparser.
    // std::string x = regparser(strings.begin(), "numberall", nullptr, nullptr, "", "");

            // Action parser right here

        //! MAJOR TESTING NEEDED.

    std::string newstring = (*target);
    auto function_on_iteration = 0;                   // some integer function on the current iteration. Default is 0. Can be changed with regexp or flag.
    static int iteration = 0;

    if (!strcmp(first, "numberall")){                 // to do: more options. default adds just number to the stated position
        char* _position = new char[strlen(fourth)-9];
        char* _totalit = new char[strlen(fifth)-11];
        std::strcpy(_position, validtokenbeforeEqual("position", fourth));    // 2nd argument return a char* OR a nullptr
        std::strcpy(_totalit, validtokenbeforeEqual("iterations", fifth)); // 2nd argument return a char* OR a nullptr
        std::cout << _position << " debug:  " << strlen(_position) << std::endl;
        std::cout << _totalit << " debug:  " << strlen(_totalit) << std::endl;


                // valid  if "position=INT" && "iterations=INT" && remainder of expressions are valid integers
                // _position and _iterations both have 2 cases: {nullptr, char*}. ..cast<int>(nullptr) ~> 0, ...cast<int>(_iterations) ~> >1
        if ( reinterpret_cast<int>(_position) && reinterpret_cast<int>(_totalit) &&
             std::regex_match(_position, std::regex("[0-9]+")) && std::regex_match(_totalit, std::regex("[0-9]+")) ){

            int position = atoi(_position);                  // converts into int; GUARANTEED no throw.
            int totaliterations = atoi(_totalit);              // converts into int; GUARANTEED no throw.

            std::string firstportion((*target).begin(), (*target).begin() + position);      // begin, begin test case?
            std::string secondportion((*target).begin() + position, (*target).end());       // >end, end test case?
                                                                                            // change this or add if-else.
            newstring = firstportion + std::to_string(iteration+function_on_iteration) + secondportion;               // concatenating before, index, and after strings

            iteration++;

            if (iteration == totaliterations)    // condition for when last file to be numbered is surpassed
                iteration = 0;                      // checks if all target files are numbered
        }
    }

    else if (!strcmp(first, "replaceall")) {
            char* _position = new char[strlen(fourth)-8];
            char* _nochars = new char[strlen(fifth)-10];

            std::strcpy(_position, validtokenbeforeEqual("position", fourth));    // 2nd argument return a char* OR a nullptr
            std::strcpy(_nochars, validtokenbeforeEqual("nochars", fifth)); // 2nd argument return a char* OR a nullptr

            int position = atoi(_position);                  // converts into int; GUARANTEED no throw.
            int nochars = atoi(_nochars);              // converts into int; GUARANTEED no throw.


            // return (*target).replace(position, nochars, std::regex(third));  // dangerous method; replaces no matter what.
                                                            // can easily write >1 file to the same name.
    }
    else {
        std::cout << "Invalid argument flag." << std::endl;
        newstring = (*target);
    }

    return(newstring);

};





int main(){

    float SELECTION = 8.0f;

    //  1. getline, then validtokenbeforeequal

    if (SELECTION == 1.0f){
        std::string argument;
        char* cargument = new char[argument.length()+1];
        std::getline(std::cin, argument);
        strcpy(cargument, argument.c_str());

        std::cout << validtokenbeforeEqual("position", cargument) << std::endl;

    }

    // 2. regex_match(string, regexp)
    else if(SELECTION == 2.0F){                         // regex for positive integer match

        std::string maybeint;
        std::getline(std::cin, maybeint);
        std::cout << std::regex_match(maybeint, std::regex(("[0-9]+")));

    }

    else if(SELECTION == 3.0F){                         // implicit casting a char* x == nullptr;
        char* a = "0";
        char* b = "\0";
        char* c = nullptr;

//        std::cout << static_cast<char>(b) << std::endl;
        std::cout << reinterpret_cast<int>(b) << std::endl;
        std::cout << reinterpret_cast<int>(c) << std::endl;


    }

    else if(SELECTION == 4.0F){
        std::string random;
        std::getline(std::cin, random);
        char* charcommand = new char[random.length()+1];
        strcpy(charcommand, random.c_str());


        std::cout << "random: " << random << "  " << random.length() << std::endl;
        std::cout << "charcommand: " << charcommand  << "   " << std::strlen(charcommand) << std::endl;

    }
    else if (SELECTION == 5.0f){
        std::string random;
        std::getline(std::cin, random);
        char* command = new char[random.length()+1];
        strcpy(command, random.c_str());
        int i = 0;

        auto x = std::strtok(command, " ");
        command = std::strtok(NULL, "");        // update command string
        i += strlen(x);
        i++;

        std::cout << command << std::endl;
        std::cout << "length: " << i << std::endl;

        auto y = std::strtok(command, " ");
        command = std::strtok(NULL, "");        // update command string
        i += strlen(y);
        i++;

        std::cout << command << std::endl;
        std::cout << "length: " << i << std::endl;

        auto z = std::strtok(command, " ");
        command = std::strtok(NULL, "");        // update command string

        std::cout << strlen(z) << std::endl;

        i += strlen(y);
        i++;
        std::cout << "length: " << i << std::endl;

        char* origcommand;
        strcpy(origcommand, command);



//        i += strlen(z);
//        i++;
//
//        std::cout << command << std::endl;
//        std::cout << "length: " << i << std::endl;



    }

    else if (SELECTION == 6.0f){            // testing regparser


        std::vector<std::string> strings;
        strings.push_back("thisisbob");
        std::cout << "strings[0]: " << strings[0] << std::endl;

            // test case 1:

        std::string a = regparser(strings.begin(), "", nullptr, nullptr, "", "");
        std::cout << "Test case 1: " << a << std::endl;

        char* position = new char[100];
        position = "position=0";

        char* iterations = new char[100];
        iterations = "iterations=0";


        std::string b = regparser(strings.begin(), "numberall", nullptr, nullptr, "position=2", "iterations=987");   // numberall test case. numberall will always have "position=XX" and "iterations=XX" as last 2 arguments
        std::cout << "Test case 2: " << b << std::endl;


//        std::string x = regparser(strings.begin(), "", nullptr, nullptr, "", "");      // test code for regparser.
//        std::cout << "Test case 3: " << x << std::endl;



    }
    else if (SELECTION == 7.0f){
//        char* blah = "string literal=0";
//        blah[0] = 'p';

//        std::string ax = "string literal=0";
//        ax[0] = 'd';
//        std::cout << ax << std::endl;

//   std::vector<std::string> *a = new std::vector<std::string>(2);
    std::vector<std::vector<std::string>> b(2);

//    b[0].emplace_back("theone");
//    std::cout << b[0].size() << std::endl;
//    std::cout << (*asdf) << std::endl;



    }
    else if (SELECTION == 8.0f){                // showing that replacing characters with a null string does nothing.


    std::string tar = "asdfag";

    char bam[5] = "\0";
    std::string cpybam(bam);

    std::cout << tar << std::endl;                  // output: "asdfag"

    tar.std::string::replace(3, 0, cpybam);         // tests show that
    std::cout << tar << std::endl;                  // output: still "asdfag"

    }

    else {};


}

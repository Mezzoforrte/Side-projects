/*********************************************************
PROJECT NAME: batchdock.h
Copyright (c) 2020-2021 Jeffery Yang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*********************************************************/

#include <vector>
#include <cstring>
#include <string>

#ifndef BATCHDOCK_H_INCLUDED
#define BATCHDOCK_H_INCLUDED

#define fcontainer std::vector                               // STL container; vector ideal since random access + iteration are only usage
#define ENTRY_RECORD std::pair<std::string, std::string>     // default entry record is pair<oldname, newname>


class dock{
private:
    int n;                              // number of files in directory
    DIR* dir;                           // dir object of files
    char* dirpath;                      // path to directory
    int first_file_index;               // position of first file whose name will be renamed. Default = 0.
    int stack_index;                    // index of the string container that currently gets displayed (i.e. most recently changed set of strings)
    bool undo_flag;                     // tracks whether a single undo has been done; vestigial if > 1 undo allowed.
    int displaystart;                   // position of file to start the display
    int ent_pp;                         // number of entires per page to display

    std::vector<fcontainer<std::string>> tempnames;  // container of original names (deprecated in version 1.0+)
    std::vector<std::string> orignames;  // container of temporary string names (deprecated in version 1.0+)
                                         // DEBUG: more data fields (i.e. metadaedittor) would require use of a "vector<pair<entrydata, stack<statechanges>>>"



    int changesapplied;                 // records number of changes; -1 if changes HAVE been applied.
    void resetdisplaystart();
    void confirmChangesForUser();
    void splitpairvector(std::vector<ENTRY_RECORD>& entries);
    void alphasort(std::vector<ENTRY_RECORD>& entries, std::string parameter = "");

public:

        // structors
    dock() : dir(nullptr), n(0), undo_flag(1), stack_index(0), dirpath(nullptr), displaystart(0), tempnames(2), ent_pp(10), first_file_index(0), changesapplied(0) {};

        // key operating functions

    template<class T = unsigned int>    // adding integer starting from 0 - n is most common case. Second most common: specify own function
    bool addfiles(const char* _dirpath);

    void closefile();
    void displaydock();
    int command_parser(char* command);
    void editall(char* first, char* second, char* third, char* fourth, char* fifth);  // first = action (replace, append), second = flag (-d, -f), third reserved, fourth reserved
    void undo();
    void undoall();
    void applychanges();                                                              // apply changes.
    void replaceall(char* options, char* third, char* pos, char* nochars);            // replaces "nochars" characters from position "pos" with "regexp"
    void numberall(char* options, char* regexp, char* position);                      // appends numbers to all files in dock. Wrapper for generic replaceall method.
    std::string regparser(std::vector<std::string>::iterator target, char* first, char* second, char* third, char* fourth, char* fifth);

    void scrollup();
    void scrolldown();
    void help_module();
    // later features:
    //  - adding >1 directory
    //  - adding file from elsewhere
    //  - add char / function


        // RESERVED functions
    void dock_edit();
    void dock_remove() {};                               // deletes chars at indices
    void dock_add() {};                                  // adds chars at indices


};

#endif // BATCHDOCK_H_INCLUDED

/*********************************************************
PROJECT NAME: MetaEditor (short for metadata Editor)
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

/************************ MANUAL *************************
Batch file renamer for the Windows command line.
General usage: features:
    1. Add files to the dock by typing in "open path/to/directory/containing/files/to/be/editted"
    2. Edit file names using key operating functions listed below.
        - changes to the file names can be observed in the dock. However, they will NOT be applied until "applychanges" is entered.
    3. Apply changes by typing in applychanges. Care; once changes are "applied", they cannot be reversed.
    4. Repeat steps 1-3 for any number of directories.

  Editting files:
    The following commands are currently available for editting file names. See "help" for more details:

    1. numberall - numbers the specified set of files in the dock according to the specified order (default is alphabetical)
        general use case: "numberall pos=[X]", where [X] is a positive integer
    2. replaceall - edits the names of the specified set of files in the dock at the specified position.
        general use case: "replaceall "string" pos=[X] char[Y]",
        where ' "string" ' is the edit string in between quotation marks
        X is the position to begin the edit string and
        Y is the number of characters that 'string' will replace starting from X.

*********************************************************/

/********************* VERSION INFO **********************
09/22/2020 - Version 1.0.
    - number set of files all at once according to alphabetical order.
    - edit names of set of files all at once
*********************************************************/


#include <iostream>
#include <cstdio>
#include <vector>
#include <dirent.h>
#include <map>
#include <cstring>
#include <memory>
#include <filesystem>
#include <regex>
#include <utility>
#include <errno.h>


#include "../lib/Common/Common.h"
#include "../lib/Batchdock.h"


#define MIN_SEL 1
#define EXITPROG 9999
#define ENTRY_RECORD std::pair<std::string, std::string> // default entry record is pair<oldname, newname>
#define STRING_IT std::vector<std::string>::iterator


         // "common" functions.

char* trimwhitespace(int flag, const char* rawstring){              // Trims leading (flag == 0), trailing(flag == 1), or both(flag == 2) whitespaces

    char bufferstring[std::strlen(rawstring)];
    char* refinedstring = 0;
    std::strcpy(bufferstring, rawstring);

      // Trim leading space.
    if (flag == 0 || flag == 2){
        char* leadch = bufferstring;
        int iteration = 0;
        while(isspace((unsigned char)(*leadch))){
            leadch++;
            iteration++;
        }

        refinedstring = new char[strlen(bufferstring)-iteration+1];
        std::strcpy(refinedstring, leadch);             // copies from where the iterator stops to refinedstring. this WILL copy just the \0 char.
    }
      // Trim trailing space.

    if (flag == 1 || flag == 2){
        char *endch = bufferstring + strlen(bufferstring) - 1;
        while(endch >= bufferstring && isspace((unsigned char)(*endch)))
            endch--;

          // Write new null terminator character
        endch[1] = '\0';

        refinedstring = new char[strlen(bufferstring)+1];
        std::strcpy(refinedstring, bufferstring);

    }

    return (refinedstring);
}

char* validateFlags(char* inputFlags, char* validFlags = nullptr){  // checks inputFlags against validFlags to see if valid.
                                                                    // Returns list of valid flags. Any invalid flag --> return (0).
    // version 1.0 unimplemented - valid flag is dummy string "valid"

        //! if any invalid flags detected, return (0);

    if (!std::strcmp(inputFlags, "-v"))
        return("\0");

    return (0);
}

char* validtokenbeforeEqual(char* prior, char* argument){       //! checks if token before "=" is as expected, and returns the second token. DONE, tested.

    if (!strcmp(argument, "\0"))
        return ("\0");

    else{
        char totaltoken[strlen(argument)];
        std::strcpy(totaltoken, trimwhitespace(1, argument));       // trims leading whitespace

        char* firsttoken = std::strtok(totaltoken, "=");

        if(!strcmp(prior, firsttoken)){

            //! If case: "position=" or "position= \anystringhere\", then consider it a NULL and: '\0'
            //! if (1) or (2), then return NULL ("\0"):
            //!    1) if strlen(argument) == strlen(prior)
            //!    2) if (trimwhitespace == true)
            //! else, tokenize the string.

            if (strlen(firsttoken)+1 != strlen(totaltoken))            // for case of "prior="
                return(std::strtok(NULL, ""));
        }
        return ("\0");
    }
}

char* validateExpression(char* regexp, char* validExps = nullptr){   // checks regexp for validity.

    if (!std::strcmp(regexp, "\0"))                     // no arguments passed
        return(0);

    if (strlen(regexp) >2 && regexp[0] == '"' && regexp[strlen(regexp)-1] == '"'){     // parses regexp here. Version 1.0: any submitted strings in "" is considered valid.

            // truncating the flanking "" marks
        char* processedexp = new char[strlen(regexp)];
        std::strcpy(processedexp, regexp);
        processedexp[strlen(processedexp)-1] = '\0';
        processedexp = &processedexp[1];

        return(processedexp);
    }
    else
        return(0);                                      // invalid argument case (i.e. {randomstring, """", etc.})

}

void setPositiontoMax(char** position, const char* targetstring){     // sets the "position" to the max possible position of the supplied string (strlength)
        char* noargsposition = new char[numDigits(std::strlen(targetstring))];
        std::strcpy(noargsposition, std::to_string(std::strlen(targetstring)).c_str());
        (*position) = noargsposition;
}

bool alphalowercmp(ENTRY_RECORD& first, ENTRY_RECORD& second){       // COMPARATOR object for alphabetically sorting the set of strings w/o rt to upper/lower case

    int i = 0;
    while (std::tolower(first.second[i]) == std::tolower(second.second[i]))
        i++;

    return(std::tolower(first.second[i]) < std::tolower(second.second[i]));
}

std::string dock::regparser(STRING_IT target, char* first, char* second, char* third, char* fourth, char* fifth){    // module that parses the individual expressions for validity, then applies the effects of the valid argument
     // first = action, second = flags, third = regexp, fourth = pos, fifth = nochars / iterations

            // initializing key variables
    std::string newstring = (*target);
    auto function_on_iteration = -1;                   // some integer function on the current iteration. Default is 0. Can be changed with regexp or flag.
    static int iteration = 1;                         // counts number of iterations


    if (!strcmp(first, "numberall")){                 // parsing argument for numberall

                // reserve space and process fifth argument for parsing
        char _totalit[abs(strlen(fifth)-11)];   // = new char[strlen(fifth)-11];
        std::strcpy(_totalit, validtokenbeforeEqual("iterations", fifth));  // 2nd argument return a char* OR a nullptr

                // 2nd - 4th argument parsing. Checks for valid arguments in arguments 2nd --> 4th.
        char* validFlags;
        char* validExpression;
        char argsposition[abs(strlen(fourth)-9)];
        char* _position = argsposition;

        // validFlags = validateFlags(second));         // do something with validFlags
                                                     // Version 1.0: does nothing.

                // validates arguments [VERSION 1.0 only searches for position= functionality.
        if (!strcmp(fourth, "\0")){                 // no fourth argument

            if (!strcmp(third, "\0")){              // no third argument

                if (!strcmp(second, "\0")){         // no argument case; will add to the end.
                    setPositiontoMax((&_position), newstring.c_str());
                }
                else {                              // second argument given; check for valid second argument.
                    if (validFlags = validateFlags(second)){                   // second argument given; check valid flag case
                            // executeFlags(validFlags)
                        setPositiontoMax((&_position), newstring.c_str());
                    }
                    else if (validExpression = validateExpression(second)){    // second argument given; check validExpression case
                            // executeExpression(validExpression)

                        setPositiontoMax((&_position), newstring.c_str());
                    }
                    else{       // second argument given, check "position=XX" case
                        std::strcpy(_position, validtokenbeforeEqual("pos", second));  // argument return a char* OR a nullptr; third guaranteed to be "\0".
                    }
                }

            }

            else {                                  // second + third arguments given; check second and third arguments
                if (validFlags = validateFlags(second)){                // second argument is valid; proceed to check third argument
                           // executeFlags(validFlags)                   //! this parses for validFlags
                    if (validExpression = validateExpression(third)){   // third argument is regexp
                            // executeExpression(validExpression)        //! this parses for regexp
                        setPositiontoMax((&_position), newstring.c_str());
                    }

                    else                                                // third argument !regexp; check "position=XX" arg
                        std::strcpy(_position, validtokenbeforeEqual("pos", third));
                }
                else                                                    // invalid second argument -> string is NULL
                    _position = "\0";

             }
        }

        else {                                       // second + third + fourth arguments provided

                if (validFlags = validateFlags(second)){                // second argument is valid; proceed to check third argument
                           // executeFlags(validFlags)                   //! this parses for validFlags
                    if (validExpression = validateExpression(third)){   // third argument is regexp and valid
                            // executeExpression(validExpression)        //! this parses for regexp
                        std::strcpy(_position, validtokenbeforeEqual("pos", fourth));  // checks fourth argument for "position=XX"
                    }
                    else
                        _position = "\0";                               // invalid third argument -> string is NULL
                }
                else                                                    // invalid second argument -> string is NULL
                    _position = "\0";
        }

                // valid  if "position=typename INT" && "iterations=typename INT" && remainder of expressions are valid integers
        if (std::regex_match(_position, std::regex("[0-9]+")) && std::regex_match(_totalit, std::regex("[0-9]+")) ){

            int position = atoi(_position);                  // converts into int; GUARANTEED no throw.
            int totaliterations = atoi(_totalit);            // converts into int; GUARANTEED no throw.

                    // invalid integer case where position > newstring.length()
             if (position > newstring.length())
                position = newstring.length();

            std::string firstportion((*target).begin(), (*target).begin() + position);      // begin, begin test case?
            std::string secondportion((*target).begin() + position, (*target).end());       // >end, end test case?
                                                                                                           // change this or add if-else.
            newstring = firstportion + std::to_string(iteration+function_on_iteration) + secondportion;    // concatenating before, index, and after strings

            iteration++;

            if (iteration == totaliterations)    // condition for when last file to be numbered is surpassed
                iteration = 1;                   // checks if all target files are numbered

        }
    }

    else if (!strcmp(first, "replaceall")) {                    // case: replaceall

            // place args into vector for iteration purposes
        std::vector<char*> copiedargs;
        typedef std::vector<char*>::reverse_iterator CHAR_IT;   // defining reverse iterator for parsing arguments

        int totalargs = 0;                                // counter keeps track of number of total arguments

        copiedargs.push_back(second);                     //! rest of algorithm depends on having at least 1 argument in container


        if (std::strlen(second)) {
            totalargs++;
            if (std::strlen(third)) {
                copiedargs.push_back(third);
                totalargs++;
                if (std::strlen(fourth)) {
                    copiedargs.push_back(fourth);
                    totalargs++;
                    if (std::strlen(fifth)){
                        copiedargs.push_back(fifth);
                        totalargs++;
                    }
                }
            }
        }

            // declaring processedargs container

        std::vector<char*> processedargs(4);            // 0 = processedflags, 1 = processedregexp, 2 = processedposition, 3 = processednochar

            // by default, processedargs initialized to NULL
        processedargs[0] = nullptr;
        processedargs[1] = nullptr;
        char argsposition[abs(strlen(fourth)-4)];
        argsposition[0] = '\0';
        processedargs[2] = argsposition;                 // (assuming valid arg positioning) if position not supplied, default will later be at end of string
        char argschar[abs(strlen(fifth)-6)];
        argschar[0] = '\0';
        processedargs[3] = argschar;                     // (assuming valid arg positioning) if nochar not supplied, default will later be 0


                // Checking to see if argument 2-5 are valid.
                // In version 1.0, the general "valid case" is: "replaceall validflag validstring pos=X chars=X", where X is some integer

                // function vector declared for purpose of argument parsing

          typedef std::vector<char *(*)(char*, char*)> FUNC_CONT;
          typedef typename FUNC_CONT::reverse_iterator FUNC_IT;


          auto validposbeforeEqual = [](char* argument, char* null = 0)->char*{
              return(validtokenbeforeEqual("pos", argument));
          };

          auto validcharbeforeEqual = [](char* argument, char* null = 0)->char*{
              return(validtokenbeforeEqual("char", argument));
          };


          FUNC_CONT argumentsparse;
          argumentsparse.push_back(validateFlags);
          argumentsparse.push_back(validateExpression);
          argumentsparse.push_back(validposbeforeEqual);            // validposbeforeEqual
          argumentsparse.push_back(validcharbeforeEqual);          // validcharsbeforeEqual


          FUNC_IT tracking_It = argumentsparse.rbegin();


                // algorithm for argument parsing
          int trueargs = 0;

                // parse from LAST (fifth) arg to FIRST (second) arg to deal with cases like ("replaceall nochar=5 validflag pos=3")

          for (CHAR_IT args_it = copiedargs.rbegin(); args_it < copiedargs.rend(); args_it++){
            if (strcmp((*args_it), "\0")) {                                 //! critical statement; don't remove.
                for (FUNC_IT fptr_It = tracking_It; fptr_It < argumentsparse.rend(); fptr_It++){
                    if ((*fptr_It) == validcharbeforeEqual){
                        if(std::strlen(std::strcpy(processedargs[3], (*fptr_It)(*args_it, 0)))){
                            tracking_It = argumentsparse.rbegin()+1;         // setting function for next argument to start at validposbeforeequal
                            trueargs++;                                      // also need to increment number of true args
                        }
                    }
                    else if((*fptr_It) == validposbeforeEqual){
                        if(strlen(strcpy(processedargs[2], (*fptr_It)(*args_it, 0)))){
                            tracking_It = argumentsparse.rbegin()+2;         // setting function for next argument to start at validateExpression
                            trueargs++;                                      // also need to increment number of true args
                        }
                    }
                    else if ((*fptr_It) == validateExpression){
                        if (processedargs[1] = (*fptr_It)(*args_it, 0)){           // apply function to argument; if there's a hit. ...
                            tracking_It = argumentsparse.rbegin()+3;         // setting function for next argument to start at validateFlags
                            trueargs++;                                      // also need to increment number of true args
                        }
                    }
                    else if((*fptr_It) == validateFlags){
                        if (processedargs[0] = (*fptr_It)(*args_it, 0)){           // apply function to argument; if there's a hit. ...
                            tracking_It = argumentsparse.rend();                       // we need to update our most recent iterator
                            trueargs++;                                      // also need to increment number of true args
                        }
                    }
                }
            }

          }

          if (!processedargs[1]){               // must have a string to replace, or else terminate.
                return(newstring);
          }

                // fill in NULL values in processedargs, if the argument string is valid (totalargs == trueargs)
          else {
            if (totalargs == trueargs){
                if (!strcmp(processedargs[2], "\0"))                // valid args, but pos not set.
                    setPositiontoMax(&(processedargs[2]), newstring.c_str());

                if (!strcmp(processedargs[3], "\0")){              //  valid args, but char not set
                    (processedargs[3])[0] = '0';
                    (processedargs[3])[1] = '\0';
                }

                    // final replacing string statement
                newstring.replace(std::atoi(processedargs[2]), std::atoi(processedargs[3]), std::string(processedargs[1]));
            }

          }



                // valid iff validflags && validregexp && "position=typename INT" && "iterations=typename INT"


            // return (*target).replace(position, nochars, std::regex(regexp));  // dangerous method; replaces no matter what.
                                                            // can easily write >1 file to the same name.
    }
    else {
        std::cout << "Invalid argument flag." << std::endl;
        newstring = (*target);
    }

    return(newstring);


};



void dock::alphasort(std::vector<ENTRY_RECORD>& entries, std::string parameter) {      // sorts dock by given parameter

    // sort DIRECTORIES on some metadata parameter

        // NAIVE - for batchrenamer only (no metadata)

        std::sort(entries.begin(), entries.begin()+first_file_index, alphalowercmp);
        std::cout <<   entries.begin()->first << "  DEBUG " << entries.begin()->second << std::endl;

    // sort FILES on some metadata parameter

        // NAIVE - for batchrenamer only (no metadata)

        std::sort(entries.begin()+first_file_index, entries.end(), alphalowercmp);



}

void dock::splitpairvector(std::vector<ENTRY_RECORD>& entries){     // takes sorted container of pair<orignames, newname>, then splits them into respective dock::origname and dock::tempnames containers.

    std::vector<std::string> new_orig;
    std::vector<std::string> new_temp;

    for (typename std::vector<std::pair<std::string, std::string>>::iterator ent_it = entries.begin(); ent_it < entries.end(); ent_it++){
        new_orig.push_back(ent_it->first);
        new_temp.push_back(ent_it->second);
    };

    orignames = new_orig;
    tempnames[stack_index] = new_temp;

}



template<class T = int>
bool dock::addfiles(const char* _dirpath) {     // addfile to current "container". returns null if addfile went wrong (errno also set).
                                                // currently restricted to only ONE folder allowed to be editted
                                                // Adds FOLDER (default = alphabetical) and THEN file (default = alphabetical)


                // processing dirpath (prior to opening the directory) to include extra '/' at the end [TESTED]
    char* processed_dirpath = new char[strlen(_dirpath)+2];  // allocate storage for new dir name
    strcpy(processed_dirpath, _dirpath);                     // copying name to buffer

    processed_dirpath[strlen(_dirpath)] = '/';
    processed_dirpath[strlen(_dirpath)+1] = '\0';

    DIR* tempdir = opendir(processed_dirpath); // opens the directory stream if valid, nullptr on invalid


    if (tempdir == 0){                 // checks if reading in filestream was successful. If not, error message, and nothing is done.
        std::cout << "Invalid directory path provided." << std::endl;
    }
    else{
        if (dir != 0)            // checks if a directory is already opened.
            closefile();         // "closes" currently open file and declares if any changes are pending.

        dir = tempdir;                    // ... then sets the directory to the valid temp directory.

                // saving the new directory name
        dirpath = processed_dirpath;

                // reading in directory
        dirent* temp;           // Dirent represents a file; temp is "buffer dirent" used for processing
        readdir(dir);           // reads (and skips) in .
        readdir(dir);           // reads (and skips) in ..

        //    "DT_DIR" (directory) == flag 16
        //    "DT_REG" = flag 0

        std::vector<std::string> dir_vector;     // directory vector
        std::vector<std::string> file_vector;    // file vector
        n = 0;


        while ((temp = readdir(dir))) {
            if (temp->d_type == DT_DIR){                                                   // checks if file is a directory
                dir_vector.emplace_back(temp->d_name);
            }
            else if (temp->d_type == DT_REG)                                               // checks if file is a file
                file_vector.emplace_back(temp->d_name);
            else { };                                                                      //  do nothing
            n++;
        };

        first_file_index = dir_vector.size();                                              // records where first file begins
        dir_vector.insert(dir_vector.end(), file_vector.begin(), file_vector.end());       // merging file_vector into dir_vector

        tempnames[stack_index] = dir_vector;
        orignames = dir_vector;

        // Current solution: time complexity = O(n) [for n emplace_backs] + 1 (set operation)
        //                   space complexity = O(n+m) [for n new files + m old files]

        undo_flag = 1;
        resetdisplaystart();
        changesapplied = 0;
    }

    return(dir);          // returns 0 if addfile not successful
}

void dock::closefile(){
    dir = nullptr;
    delete dirpath;                 // deletes the string allocated for storing file name
    dirpath = nullptr;              // sets dirpath to nullptr

    confirmChangesForUser();
}

void dock::editall(char* first, char* second, char* third, char* fourth, char* fifth) {
    // first = action, second = options, third = regexp, fourth = pos, fifth = characters

    char* extraargument5 = fifth;                                // variable for passing "extra argument" to regparser, by case of need.

    std::vector<ENTRY_RECORD> temp_vector;               // temporary buffer vector to simultaneously sort both the origname & newname by a specified total order on the set of newnames.

    if (!strcmp(second, "-d")){                          // directories only
        if (!strcmp(first, "numberall")){                // preprocessing for numberall
            std::string i = std::to_string(first_file_index);
            extraargument5 = new char[11+i.length()];
            std::strcpy(extraargument5, "iterations=");
            std::strcat(extraargument5, i.c_str());                     // [TEMP]
        }
        else {   };         // reserved for future arguments

        for (STRING_IT temp_it = tempnames[stack_index].begin(), orig_it = orignames.begin();
         temp_it < tempnames[stack_index].begin()+first_file_index; temp_it++, orig_it++){
            temp_vector.emplace_back(std::make_pair((*orig_it), regparser(temp_it, first, second, third, fourth, extraargument5)));
        }

        for (STRING_IT  temp_it = tempnames[stack_index].begin()+first_file_index, orig_it = orignames.begin()+first_file_index;
            temp_it < tempnames[stack_index].end(); temp_it++, orig_it++){
            temp_vector.emplace_back(std::make_pair((*orig_it), (*temp_it)));
        }

                    // checks if changes are applied
        if(temp_vector.begin()->first != temp_vector.begin()->second)
            changesapplied++;
    }

    else if (!strcmp(second, "-f")) {                    // files only
        if (!strcmp(first, "numberall")){                // preprocessing for numberall
            std::string i = std::to_string(n-first_file_index);
            extraargument5 = new char[11+i.length()];
            std::strcpy(extraargument5, "iterations=");
            std::strcat(extraargument5, i.c_str());                     // [TEMP]
        }
        else {   };  // reserved for future arguments

        for (STRING_IT  temp_it = tempnames[stack_index].begin(), orig_it = orignames.begin();
            temp_it < tempnames[stack_index].begin()+first_file_index; temp_it++, orig_it++){
            temp_vector.emplace_back(std::make_pair((*orig_it), (*temp_it)));
        }

        for (STRING_IT  temp_it = tempnames[stack_index].begin()+first_file_index, orig_it = orignames.begin()+first_file_index;
            temp_it < tempnames[stack_index].end(); temp_it++, orig_it++){
            temp_vector.emplace_back(std::make_pair((*orig_it), regparser(temp_it, first, second, third, fourth, extraargument5)));
        }

                    // checks if changes are applied
        if((temp_vector.begin()+first_file_index)->first != (temp_vector.begin()+first_file_index)->second)
            changesapplied++;

    }

    else {                                               // all files / directories
        if (!strcmp(first, "numberall")){                // preprocessing for numberall
            std::string i = std::to_string(n);
            extraargument5 = new char[11+i.length()];
            std::strcpy(extraargument5, "iterations=");
            std::strcat(extraargument5, i.c_str());
        }
        else {   };  // reserved for future arguments



            // adding pair<origname, tempname> to the "buffer" tempvector
    for (STRING_IT temp_it = tempnames[stack_index].begin(), orig_it = orignames.begin();
        temp_it < tempnames[stack_index].end(); temp_it++, orig_it++){
        temp_vector.emplace_back(std::make_pair((*orig_it), regparser(temp_it, first, second, third, fourth, extraargument5)));
    }



                        // checks if changes are applied

            if(temp_vector.begin()->first != temp_vector.begin()->second)
                changesapplied++;
    }

    stack_index = !stack_index;         // bool if only 2 stages allowed;
                                        //   if >2 stage allowed, change to ++ instead

    alphasort(temp_vector);                  // sorts based on alphabetical order
    splitpairvector(temp_vector);            // splits up pair; origname container and temp_names container updated to reflect name change & sor

    undo_flag = 0;                           // change -> undo option available.

};

void dock::confirmChangesForUser(){      // displays message indicating if changes were applied.
    if (changesapplied == -1)
        std::cout << "Changes were successfully applied." << std::endl;
    else if (changesapplied > 1)
        std::cout << "Changes still pending." << std::endl;
    else
        std::cout << "Nothing was changed." << std::endl;    // no message needed, but for robustness sake.
}                                                            // Cases: - right after addfile, undo to original name, "dummy" edit

void dock::applychanges(){                // applies changes

    if(changesapplied > 0){             // changes applied normally starts at 0 and increments by 1 everytime an edit operation is made.
        changesapplied = -1;            // -1 is flag that changes HAVE been applied. otherwise, each "edit" op inc. it by 1.

        for (STRING_IT orig_it = orignames.begin(), temp_it = tempnames[stack_index].begin(); orig_it < orignames.end(); orig_it++, temp_it++){


            std::string _origname = (*orig_it);
            std::string _newname = (*temp_it);
            int oldlength = _origname.length();
            int newlength = _newname.length();
            int filepathlength = strlen(dirpath);


            char refinedoldname[filepathlength + oldlength];
            char refinednewname[filepathlength + newlength];
            strcpy(refinedoldname, dirpath);
            strcpy(refinednewname, dirpath);

            strcat(refinedoldname, _origname.c_str());              // changes "oldname" --> "path/to/file/oldname"
            strcat(refinednewname, _newname.c_str());               // changes "newname" --> "path/to/file/newname"

            std::rename(refinedoldname, refinednewname);

            (*orig_it) = (*temp_it);                                // updating set of original names to new names that were "just applied"

        }


    }

    confirmChangesForUser();
    changesapplied = 0;             // resets changes applied.
    undo_flag = 1;                  // sets undo flag back to 1.
}

void dock::undo() {                 // undo undoes the last "edit" operation. Version 1.0 MAX undo = 1.
    if (undo_flag == 0){
        stack_index = !stack_index;     // bool if only 2 stages allowed;
                                        // if >2 stage allowed, change to ++ instead; i.e. to emulate a STACK..
        undo_flag = !undo_flag;

        changesapplied--;
        resetdisplaystart();
    }

}

void dock::undoall() {              // undoes all changes

    if (changesapplied > 0){
        tempnames[stack_index] = orignames;
    }

    undo_flag = 1;                      // necessary so no more undo gets called
    changesapplied = 0;
    resetdisplaystart();

}

void dock::scrollup(){              // scrolls set of entries down 1 page
    if (displaystart - ent_pp >= 0)
        displaystart -= ent_pp;
}

void dock::scrolldown(){            // scrolls set of entries down 1 page
    if (displaystart + ent_pp < n)
        displaystart += ent_pp;

}

void dock::resetdisplaystart(){     // resets the 1st entry for display to position "0", a.k.a. "1."
    displaystart = 0;
}

void dock::numberall(char* options, char* regexp, char* position) {   // numbers all some subset of files ind ock according to (default is alphabetical) order
    std::cout << position << std::endl;     // debug
    editall("numberall", options, regexp, position, nullptr);
}

void dock::replaceall(char* options, char* third, char* pos, char* nochars) {
    editall("replaceall", options, third, pos, nochars);
}

int dock::command_parser(char* command){

        // String tokenizer
    std::vector<char*> token;
    int commandlength = strlen(command);
    char commandcopy[commandlength+1];
    commandcopy[commandlength+1] = '\0';
    strcpy(commandcopy, command);


    if (commandlength > 0){                // check for case where nothing is entered
        int i = 0;                         // counter for iterating through whitespace in

        for (; i < commandlength; i++){
            if (!isspace(commandcopy[i])){              // ensures token starts on non whitespace. Witness: Case " string", "   " throws error.
                if(token.size() == 0)                   // extracts 1st token
                    token.push_back(std::strtok(commandcopy, " \t\0"));
                else                                    // extracting token >1
                    token.push_back(std::strtok(NULL, " \t\0"));

                i += strlen(token.back());
            }
        }
    }
    else
        token.push_back(command);

    while (token.size() < 5){                   // ensures no NULL arguments when checked, by setting all argument slots to '\0'
        char* temp = new char[1];
        temp[0] = '\0';
        token.push_back(temp);
    };

    if (!strcmp(token[0], "open")){
            addfiles(token[1]);                // passes argument to addfiles; doesn't do anything if the filepath is invalid.
            displaydock();
            return(1);
    }

    if (dir == 0)
        std::cout << "Nothing is opened" << std::endl;  // will not parse arguments if no file has been opened

    else {

        if (!strcmp(token[0], "up")) {
            scrollup();
            displaydock();
        }
        else if (!strcmp(token[0], "down")){
            scrolldown();
            displaydock();
        }
        else if (!strcmp(token[0], "undo")){
            undo();
            displaydock();
        }
        else if (!strcmp(token[0], "undoall")){
            undoall();
            displaydock();
        }
        else if (!strcmp(token[0], "replaceall")){
            replaceall(token[1], token[2], token[3], token[4]);
            displaydock();
        }
        else if (!strcmp(token[0], "numberall")){
            std::cout << token[3] << std::endl;
            numberall(token[1], token[2], token[3]);
            displaydock();
        }
        else if (!strcmp(token[0], "applychanges")){
            applychanges();
            displaydock();
        }
        else if (!strcmp(token[0], "exit")){
            return (EXITPROG);
        }
        else {
                std::cout << "Invalid option." << std::endl;
        }
    }

    return(0);
}


void dock::help_module() {                          // To be implemented.
    // displays the help. List of commands.

    // 1) scroll dwn
    // 2) scroll up
    // 3) open directory     // main functionality allows for reading in of ONE folder at once to edit data
    // 4) clear directory
    // 5) view options
    //     - entries per page: 10, 20, 30
    //     - metadata view on / off
    // 6) turn renamer functionality on/off
    // 7) turn metadata editor functionality on/off
    // 8) start renaming "module"
    // 9) start metadata editor
}


void dock::displaydock() {

        // "clear original module here". To be implemented.

    int remain = n - ent_pp - displaystart;                 // counts entries remaining after ent_pp entires are displayed.
    int i = 0;                                              // iteration tracker
    int i_coefficient = displaystart+1;                     // coefficient to adjust iteration tracker by for the display
    STRING_IT string_it = tempnames[stack_index].begin()+displaystart;             // initializing vector iterator

    if (displaystart > 0)
        std::cout << "\n    -  " << displaystart << " entries above - " << std::endl << std::endl;          // shows entries above

    while (string_it < tempnames[stack_index].end() && i < ent_pp){          // iterate until ent_pp iterations or until reach end of container
        std::cout << i+i_coefficient << ".   " << *string_it << "\n";
        i++, string_it++;
    }

    for (; i < ent_pp; i++) {                                // iterate unfilled entries with blank entries
        std::cout << i+i_coefficient << "." << "\n";
    };

    if (remain > 1)
        std::cout << "\n    -  " << remain << " entries below -";

    std::cout << "\n\n";

}



int main (int argc, char* argv[]) {

    dock DOCK;


        // initial display
    std::cout << "          - METEDITOR -      \n\n";
    DOCK.displaydock();
    std::cout << "  Please make your selection (help for more options): \n\n";


    std::string command;
    char* charcommand = nullptr;
    int status;


    do{                         // continuous loop until exit is called
        std::getline(std::cin, command);
        char charcommand[command.length()+1];

        strcpy(charcommand, command.c_str());

        status = DOCK.command_parser(charcommand); // parse command
    }
    while (status != EXITPROG);

    return (1);


}




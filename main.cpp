
/* main.cpp
 * Liliia Pavlishin
 * This is a model of a UNIX-like file system using a threaded tree.
 * It has 2 types of nodes: directories and files.
 * This program supports mkdir, rmdir, chdir, ls, lsr, mkfile, rmfile, pwd, and Q (for quit) commands.
 *
 * This program reads commands from a file converts it to string and passes it into a file system as a string
 * together with output stream.
 */

//#include <iostream>
#include <fstream>
#include "file_system.h"
using namespace std;

int main(int argc, char *argv[])
{

    FILE_SYSTEM fs;
    string command;
    bool exit = false;

    if(argc < 3)
    {
        cout << "ERROR: Not enough parameters!";
        return -1;
    }

    ifstream fin(argv[1]);
    ofstream fout(argv[2]);

    // use this instead of entering arguments
//    ifstream fin("fileTest.txt");
//    ofstream fout("output.txt");


    if(!fin)
    {
        cout << "ERROR: Unable to open 'fileTest.txt' file!" << endl;
        return -1;
    }
    if(!fout)
    {
        cout << "ERROR: Unable to open 'Output.txt' file!" << endl;
        return -1;
    }
    while (getline(fin, command) && exit == false)
    {
        exit = fs.process_command(command, fout);                  // PUT A COUT instead of FOUT if you want to output the result on the screen
    }
    fin.close();
    fout.close();
    return 0;
}
/* file_system.h
 * Liliia Pavlishin
 * This header file contains declarations for the file system
 */

#ifndef FILE_SYSTEM_H_INCLUDED
#define FILE_SYSTEM_H_INCLUDED
#include <iostream>
//#include <fstream>

struct NODE
{
    char t;
    std::string name;
    NODE* parent, *child, *sibling;
};

class FILE_SYSTEM
{
private:
    NODE* root, *cwd;
    void wipe_sys(NODE* node);
    void make_dir(std::string name, std::ostream &fout);
    void rm(std::string name, char type, std::ostream& fout);
    void make_file(std::string name, std::ostream &fout);
    void print_cwd(NODE* node, std::ostream &fout);
    void change_dir(std::string path, std::ostream &fout);
    void print_path(NODE* ptr, std::ostream &fout);
    void print_all(NODE* ptr, std::ostream &fout);
    void wipe_all(NODE* node);

public:
    FILE_SYSTEM();      // constructor
    ~FILE_SYSTEM();      // destructor
    bool process_command(std::string command, std::ostream &fout);      // receives that command and processes it
    // returns 0 if need to exit the program
};
#endif // FILE_SYSTEM_H_INCLUDED
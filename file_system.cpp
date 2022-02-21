
/* file_system.cpp
 * Liliia Pavlishin
 * This program is a model of a UNIX-like file system
 */
#include "file_system.h"
#include <string>
#include <iostream>

// constructor
FILE_SYSTEM::FILE_SYSTEM()
{
    NODE* root_node = new NODE;
    cwd = root = root_node;
    root->name = "/";
    root->t = 'd';
    root->parent  = root;
    root->child = nullptr;
    root->sibling = nullptr;

}

// destructor
FILE_SYSTEM::~FILE_SYSTEM()
{
    wipe_sys(root);
}

// wipes the system clean
void FILE_SYSTEM::wipe_sys(NODE* node)
{
    wipe_all(node->child);
    delete root;
    root = nullptr;
}

// Returns true for exiting the program or false to continue running
bool FILE_SYSTEM::process_command(std::string command, std::ostream &fout)
{
    std::string cmd, name;
    int index;
    index = command.find(" ");                  // index = -1 if there is only one word in the command otherwise gives an index where space is located
    // if more than one word in a command (mkdir name, rmdir name, chdir name, mkfile name, rmfile name.)
    if(index != -1)
    {
        cmd = command.substr(0, index);
        name = command.substr(index+1);
    }
    // if command consists only of one word (ls, lsr, pwd, Q)
    else
    {
        cmd = command;
        name = "NA";
    }
    fout << '*' << command << std::endl;
    if(cmd == "mkdir")                                        // make a new directory if dir with that name does not exist, otherwise ignore.
    {
        fout << "Creating a directory '" << name << "' in '" << cwd->name << "' directory" << std::endl;
        if(name == "/")                                       // root directory already exists, new dir cannot be root
        {
            fout << "Cannot create a directory with '/' name" << std::endl;
            return false;
        }
        make_dir(name, fout);
    }
    else if(cmd == "rmdir")                                  // remove the directory with that name
    {
        fout << "Removing the directory '" <<  name << "' from '" << cwd->name << "'" << std::endl;
        if(name == "/")                                      // removing root directory is not allowed
        {
            fout << "ERROR: Cannot delete '/' directory! " << std::endl;
            return false;
        }
        rm(name, 'd', fout);
    }
    else if(cmd == "chdir")                                 // change directory
    {
        fout << std::endl;
        change_dir(name, fout);
    }
    else if(cmd == "ls")                                    // list all files in the dir
    {
        fout << "Listing all files and directories in the current working directory '" << cwd->name << std::endl;
        print_all(cwd, fout);
    }
    else if(cmd == "lsr")                                   // list all files recursively
    {
        fout << "Recursively listing all files and directories in '" << cwd->name << "' directory and all subdirectories " << std::endl;
        print_all(root, fout);
        fout << std::endl;

    }
    else if(cmd.compare("mkfile") == 0)                 // create new file with the indicated name
    {
        fout << "Creating a file with '" << name << "' name in the '" << cwd->name << "' directory" << std::endl;
        if(name == "/")
        {
            fout << "Cannot create a file with '/' name" << std::endl;          // cannot have file with the name symbol for root
            return false;
        }
        make_file(name, fout);
    }
    else if(cmd == "rmfile")                               // remove file with the name in current directory
    {
        fout << "Removing file with ' " << name << "' from '" << cwd->name << "' directory" << std::endl;
        rm(name, 'f', fout);
    }
    else if(cmd == "pwd")                                   // print full path from root to current directory
    {
        fout << "\nPriting a path from '/' to '" << cwd->name << "' directory" << std::endl;
        print_path(cwd, fout);
        fout << std::endl;
    }
    else if(cmd == "Q")                                     // quit the program
    {

        fout << "Quitting the program!\nListing full contents of the file system:" << std::endl;
        print_all(root, fout);
        fout << "\n\nEnd of run" << std::endl;
        std::cout << "\nClosing input and output files...\nExiting the program..." << std::endl;
        return true;
    }
    else                                                    // if command is not recognized throwing an error message
    {
        fout << "Command '" << cmd << " was not recognized!" << std::endl;
    }
    fout << std::endl;
    return false;
}

// function handling mkdir command
void FILE_SYSTEM::make_dir(std::string name, std::ostream &fout)
{
    NODE* ptr = cwd->child;
    NODE* node = new NODE;
    node->name = name;
    node->t = 'd';
    node->parent = cwd;   // hook onto node's parent
    node->sibling = node->child = nullptr;

    // error handling. Cannot create a directory within a file
    if(cwd->t == 'f')
    {
        fout << "ERROR: Cannot make a directory in the file!" << std::endl;
        delete node;
        return;
    }
    else if(ptr == nullptr)             // if the directory is empty, put the new dir here
    {
        cwd->child = node;
    }
    else
    {
        if(ptr->t == 'd')
        {
            // while this pointer has a sibling, and this sibling is not a file, and the new dir is not in correct space alphabetically
            // and there is no directory with that same name go through the loop
            while(ptr->sibling != nullptr && ptr->sibling->t != 'f' && ptr->sibling->name < node->name && ptr->name != node->name)
            {
                ptr = ptr->sibling;
            }
            if(ptr->name == node->name)                                            // if a dir with a same name is encountered
            {
                fout << "ERROR: Directory with '" << node->name << "' already exists in the directory!" << std::endl;
                delete node;
                return;
            }
            else if(ptr->sibling == nullptr || ptr->sibling->t == 'f')            // if there is only one dir in the list
            {
                if(ptr->name > node->name)                                        // if the new dir belong in front of this dir
                {
                    node->sibling = ptr;
                    cwd->child = node;
                }
                else                                                              // if the new dir belongs after this dir
                {
                    node->sibling = ptr->sibling;
                    ptr->sibling = node;
                }
            }
            else if(ptr->name > node->name)                                        // insert in front
            {
                node->sibling = ptr;
                cwd->child = node;
            }
            else                                                                    // if node belongs in the middle or the end
            {
                node->sibling = ptr->sibling;
                ptr->sibling = node;
            }
        }
        else                                                                        // if the first thing on the list is a file, insert in front
        {
            node->sibling = ptr;
            cwd->child = node;
        }
    }
    return;
}


// function handling rmdir, rmfile commands as well as clearing the whole tree
void FILE_SYSTEM::rm(std::string name, char type, std::ostream& fout)
{
    // there are nothing in this dir, cannot delete anything from it, return
    if(cwd->child == nullptr)
    {
        fout << "This directory '" << cwd->name << "' is empty!" << std::endl;
        return;
    }
    NODE* ptr = cwd->child;
    NODE* prev_ptr = ptr;
    while(ptr != nullptr && ptr->name != name)                         // while the dir is not empty and the name is not found
    {
        prev_ptr = ptr;
        ptr = ptr->sibling;
    }
    // if dir is not empty, and it has the name requested but the type is wrong and the type is a file
    if(ptr != nullptr && ptr->name == name && ptr->t != type && type != 'd')
    {
        prev_ptr = ptr;
        ptr = ptr->sibling;
        while(ptr != nullptr && ptr->name != name)                              // traverse till you find the correct dir
        {
            prev_ptr = ptr;
            ptr = ptr->sibling;
        }
    }
    // if the dir not empty, and name and type matches
    else if(ptr != nullptr && ptr->name == name && ptr->t == type)
    {
        // wipe everything in this dir
        wipe_all(ptr->child);
        // delete this dir
        if(cwd->child->name == ptr->name && cwd->child->t == ptr->t)        // deleting the first kid
        {
            cwd->child = ptr->sibling;
        }
        else                                                                // deleting in the middle or end
        {
            prev_ptr->sibling = ptr->sibling;
        }
        delete ptr;
        ptr->child = ptr->sibling = ptr->parent = nullptr;                  // sanitize memory
    }
    // if didn't find what you were looking for, or the name is a match but the type is not
    else if(ptr == nullptr || ptr->sibling == nullptr || (ptr->name == name && ptr->t != type))
    {
        fout << "ERROR: Cannot remove nonexistent ";
        if(type == 'f')
        {
            fout << "file!" << std::endl;
        }
        else
        {
            fout << "directory!" << std::endl;
        }
    }
    // if the name and type match
    else if(ptr->name == name && ptr->t == type)
    {
        wipe_all(ptr->child);
        if(cwd->child->name == ptr->name && cwd->child->t == ptr->t)        // if deleting the first kid
        {
            cwd->child = ptr->sibling;                                      // make the next file first and delete first
        }
        else                                                                // if deleting in the middle or end kid
        {
            prev_ptr->sibling = ptr->sibling;
        }
        delete ptr;                                                         // delete that file or dir
        ptr->child = ptr->sibling = ptr->parent = nullptr;                  // sanitize memory
    }
    // if the name or type is not a match output the error message
    else
    {
        if(type == 'f')
        {
            fout << "File ";
        }
        else
        {
            fout << "Directory ";
        }
        fout << name;
        if(type == 'f')
        {
            fout << ".file";
        }
        fout << "Does not exist in the directory!" << std::endl;
    }
    return;
}

// function handling 'chdir' commnd which changes the cwd (current working dir)
void FILE_SYSTEM::change_dir(std::string path, std::ostream &fout)
{
    fout << "Changing current working directory from '" << cwd->name << "' to '";
    bool success = true;
    NODE* temp_cwd = cwd;
    NODE* ptr = nullptr;
    std::string token;
    unsigned int index = 0;
    int end_index = 0;
    // parsing path
    if(path[0] == '/')
    {
        temp_cwd = root;
        index++;
    }
    while(index < path.length())
    {
        // parsing the string
        end_index = path.find("/", index+1) - index;                        // end index of the name
        token = path.substr(index, end_index);                                      // token holds a dir name
        index += (token.length() + 1);                                              // increment for the start of next name

        if(token == "..")                                                           // if requested to get to the upper level of dir
        {
            // error handling for going out of root dir
            if(temp_cwd->name == "/")
            {
                fout << "ERROR: Cannot go higher than '/'!" << std::endl;
                success = false;
                return;
            }
            temp_cwd = temp_cwd->parent;                                            // go up ones in the dir
        }
        else if(token == ".")                                                       // this is here just for readability
        {
            // do nothing
        }
        // if it is a name of a dir go in
        else
        {
            ptr = temp_cwd->child;
            if(ptr == nullptr)                                                       // if temp_cwd is empty
            {
                fout << "\nERROR: directory '" << token << "' does not exist in this directory!" << std::endl;
                success = false;
                return;
            }
            // traverse until the end of the list or no more dirs, or didn't find the name
            // and did not alphabetically reach the point where the dir is not there anymore
            while(ptr->sibling != nullptr && ptr->t != 'f' && ptr->sibling->t != 'f' && ptr->name != token && ptr->name < token)
            {
                ptr = ptr->sibling;                                                 // advance the pointer
            }
            if(ptr->name == token && ptr->t != 'f')                                 // dir found, go in
            {
                temp_cwd = ptr;
            }
            else if(ptr->name == token && ptr->t == 'f')                            // error handling: if the name match but it is a file
            {
                fout << "ERROR: Cannot change cwd to a " << ptr->name << ".file file!" << std::endl;
                success = false;
            }
            else if(ptr->sibling == nullptr || ptr->sibling->t == 'f')              // error handling: dir not found
            {
                fout << "ERROR: No such dir/file found in this '" << temp_cwd->name << "' directory!" << std::endl;
                success = false;
            }
            else                                                                    // this should not ever happen, here for doomsday
            {
                fout << "\nFATAL ERROR IN change_directory function!" << std::endl;
                success = false;
                break;
            }
        }
    }
    // if successful in traversing the whole path, change the dir
    if(success == true)
    {
        cwd = temp_cwd;
        fout << cwd->name << "'" <<  std::endl;
    }
    return;
}

// function handling 'ls' command. Lists all the files and dirs in cwd
void FILE_SYSTEM::print_cwd(NODE* node, std::ostream &fout)
{
    if(cwd->child == nullptr)
    {
        fout << "ERROR: This directory is empty!" << std::endl;
        return;
    }

    NODE* ptr = cwd->child;
    fout << cwd->name << '/';

    while(ptr != nullptr)
    {
        fout << ptr->name;
        if(ptr->t == 'f')
        {
            fout << ".file";
        }
        fout << '/';
        ptr = ptr->sibling;
    }
    fout << std::endl;
    return;
}

// function handling 'lsr' command which recursively lists all files and dir and all subcategories
void FILE_SYSTEM::print_all(NODE* ptr, std::ostream &fout)
{
    if(ptr == nullptr)
    {
        fout << "ERROR: There is nothing to print!" << std::endl;
        return;
    }

    static int c = 0;                                                   // keeps track of levels
    fout << std::endl;

    for (int i = 0; i < c; i++)                                         // prints spaces
    {
        fout << "   ";
    }
    fout << ptr->name;                                                  // print a node
    if(ptr->t == 'f')                                                   // add a proper extension
    {
        fout << ".file";
    }
    if(ptr->child != nullptr)                                           // if there is a child go down
    {
        c++;                                                            // add a level when accessing a child
        print_all(ptr->child, fout);                                // go into child dir and print everything inside
        c--;                                                           // subtract a level when returning to the parent
    }
    if(ptr->sibling != nullptr)                                         // if there is a sibling go right
    {
        print_all(ptr->sibling, fout);
    }
    return;
}

// function handling 'mkfile' command. Creates new file with the specified name
void FILE_SYSTEM::make_file(std::string name, std::ostream &fout)
{
    NODE* ptr = cwd->child;
    NODE* node = new NODE;
    node->t = 'f';
    node->name = name;
    node->parent = cwd;                                                     // hook onto node's parent
    node->sibling = node->child = nullptr;

    if(cwd->t == 'f')                                                       // cannot add anything to a file
    {
        fout << "ERROR: Cannot make a file in a file!" << std::endl;
        delete node;
        return;
    }
    else if(ptr == nullptr)                                                  // the list is empty
    {
        cwd->child = node;
    }
    else                                                                    // there is something already in this dir
    {
        while( ptr->sibling != nullptr && ptr->sibling->t != 'f')           // while file or the end of the list is not reached
        {
            ptr = ptr->sibling;                                             // advance the pointer
        }
        // error handling for when there is already a file with the same name
        if(ptr->sibling != nullptr && ptr->sibling->name == node->name && ptr->t != 'd')
        {
            fout << "ERROR: file with '" << node->name << "' already exists in this director!" << std::endl;
            delete node;
            return;
        }
        // if the end of the list is reached (list contains only)
        if(ptr->sibling == nullptr)
        {
            // if name of a node is < name of the ptr, attach to front
            if(node->name < ptr->name && ptr->t != 'd')
            {
                node->sibling = ptr;
                cwd->child = node;
            }
            // if name of node is > name of ptr, attach to the end
            else
            {
                ptr->sibling = node;
            }
            return;
        }
        // traverse while not the end of the dir and next name is lexicographically smaller than the requested name
        // and the next name does not match the requested name
        while(ptr->sibling != nullptr && ptr->sibling->name < node->name && ptr->sibling->name != node->name)                          // while did not find where to put the node advance the ptr
        {
            ptr = ptr->sibling;
        }
        // error handling for when the file with the same name already exists
        if(ptr->sibling != nullptr && ptr->sibling->name == node->name && ptr->t != 'd')
        {
            fout << "ERROR: file with '" << node->name << "' already exists in this directory!" << std::endl;
            delete node;
            return;
        }
        if(ptr->t == 'd')                                               // insert at the end
        {
            node->sibling = ptr->sibling;
            ptr->sibling = node;
        }
        else if(ptr->sibling != nullptr && ptr->sibling->name > node->name)
        {
            node->sibling = ptr->sibling;
            ptr->sibling = node;
        }
        else if(ptr->sibling == nullptr && ptr->name < node->name)       // insert at the back
        {
            ptr->sibling = node;
        }
        else //if(ptr->name > node->name)                           // inset at front
        {
            node->sibling = ptr;
            cwd->child = node;
        }
    }
    return;
}

// function handling 'pwd' command, printing full path from root to current working directory
void FILE_SYSTEM::print_path(NODE* ptr, std::ostream &fout)
{
    if(ptr->name != "/")
    {
        print_path(ptr->parent, fout);
    }
    fout << ptr->name;
    if(ptr->name != "/")
    {
        fout << '/';
    }
    return;
}

// this function deletes everything starting at the node passed through the parameter
void FILE_SYSTEM::wipe_all(NODE* node)
{
    if(node == nullptr)                                 // if nothing is there return
    {
        return;
    }
    if(node->sibling != nullptr)                        // if there is another file call wipe_all() on it
    {
        wipe_all(node->sibling);
    }
    if(node->child != nullptr)                          // if there is a deeper level call wipe_all() on it
    {
        wipe_all(node->child);
    }
    delete node;                                        // delete the remaining node
    node = nullptr;                                     // sanitize memory
    return;
}
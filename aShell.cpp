#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <termios.h>
#include <ctype.h>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

void ResetCanonicalMode(int fd, struct termios *savedattributes){
    tcsetattr(fd, TCSANOW, savedattributes);
}
bool ff_recurse(const char* current_dir, const char* filename, string last_dir, bool First){
    // base case: found a file in directory!
    // recursive case: found another directory, add to path in list of paths
    DIR *ff_dir;
    struct dirent *ff_struct;
    string str_current_dir(current_dir);
    string str_filename(filename);
    string ff_d_name;
    

    if ((str_current_dir.compare(".") == 0) || (str_current_dir.compare("..") == 0)){
        return false;
    }
    //cout << "Current directory ff: " << str_current_dir << "\n";
    if ((ff_dir= opendir(current_dir)) == NULL)
    {
        //write(1, "ERROR", 5);
        // reached a file-- base case
        if (str_current_dir.compare(str_filename) == 0) // found a match
        {
            cout << "WE got a hit\n";
            cout << "last dir: " << last_dir << "\n";
            cout << "hello\n";

            // for (int j = 0; j < str_current_dir.size(); j++)
            // {
            //     write(1, &current_dir[j], 1);
            // }
            return true; 
        }
        else{
            return false; // reached non-filename file
        }
    }
    else{ // reached a directory
        //last_dir = last_dir + "/";
        //last_dir = last_dir + str_current_dir;
        string temp;
        cout << "last_dir asdf: "<< last_dir << "\n";
        if (First)
        {
            temp = last_dir;
        }
        else
        {
            temp = last_dir + "/" + str_current_dir;
        }
        //string temp = last_dir + "/" + str_current_dir;
        cout << "str curr dir: " << str_current_dir << "\n";
        // for (int k = 0; k < str_current_dir.size(); k++)
        // {
        //     write(1, &current_dir[k], 1);
        // }
        // write(1, "/", 1);
        while ((ff_struct = readdir(ff_dir)) != NULL)
        {
            int i = 0;
            while (ff_struct->d_name[i] != '\0')
            {
                ff_d_name.push_back(ff_struct->d_name[i]);
                i++;
            }
            ff_recurse(ff_struct->d_name, filename, temp, false);
        }
        return false;
    }
   
}
void SetNonCanonicalMode(int fd, struct termios *savedattributes){
    struct termios TermAttributes;
    char *name;
    
    // Make sure stdin is a terminal.
    if(!isatty(fd)){
        fprintf (stderr, "Not a terminal.\n");
        exit(0);
    }
    
    // Save the terminal attributes so we can restore them later.
    tcgetattr(fd, savedattributes);
    
    // Set the funny terminal modes.
    tcgetattr (fd, &TermAttributes);
    TermAttributes.c_lflag &= ~(ICANON | ECHO); // Clear ICANON and ECHO.
    TermAttributes.c_cc[VMIN] = 1;
    TermAttributes.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSAFLUSH, &TermAttributes);
}
void printString(string Source)
{
    for(int i = 0; i < Source.size(); i++)
    {
        write(1, &Source[i], 1);
    }
    return;
}

string truncateString(string Line)
{
    string New;
    for (int k = Line.size() - 1; k >= 0; k--)
    {
        if (Line[k] == '/')
        {
            New = "/.../";
            //cout << "Initial k is " << k << "\n";
            for (int j = k + 1; j < Line.size(); j++)
            {
                New.push_back(Line[j]);
            }
            break;

        }
        else
        {
            continue;
        }

    }
    return New;
}



int main ()
{
    char buffer;
    string command;
    char directory[100];
    bool Arrow_One = false;
    bool Arrow_Two = false;
    string FirstPart;
    string temp2;
    string** past_commands;
    string path;
    string dr;
    char* cwd;
    char* ptr;
    char* parsed;
    //int size = 0;
    char temp;
    struct termios Attributes;
    bool loop_exit = false;
    char endChar = '%';
    char newLine = '\n';
    const char* new_path;
    
    
    SetNonCanonicalMode(0, &Attributes);
    while (!loop_exit)
    {
        ptr = getcwd(directory, 50);
        dr = string(ptr);

        if (dr.size() >= 16)
        {
            string temp = truncateString(dr);
            printString(temp);
            
        }
        else
        {
            printString(dr);
        }

        write(1, &endChar, 1);
        write(1, " ", 1);
        //Print the current directory
        
        int dummy = 0;
        while(read(0, &buffer, 1) > 0)
        {
           // cout << "hi!!!! \n";
        
            if(int(buffer) ==  127)
            {
                //cout << "Delete key!\n";
                write(1, "\b \b", 3);
                continue;
            }
        
           //Implement the backspace
            if (int(buffer) == 27) //Check to see if it is the arrow
            {
                read(0, &buffer, 1);
                if (int(buffer) == 91)
                {
                    read(0, &buffer, 1);               
                    if(int(buffer) == 65)
                    {
                        cout << "This was the up arrow!\n";
                    }
                    else if (int(buffer) == 66)
                    {
                        cout << "This was the down arrow \n";
                    }
                }
            }

            if (buffer == '\n')
            {
                break;
                
            }
            else
            {
                command.push_back(buffer);
                
            }
            write(1, &buffer, 1);
            dummy++;
            
        }
        //cout << "The size is " << size << "\n";

        int i = 0;
        while (command[i] != ' ' && i < command.size())
        {
            
            //cout << "Pushing back" << command[i] << "AHHH\n";
            FirstPart.push_back(command[i]);
            i++;
            
        }//Get length of the command

        // write(1, "\n", 1);
        
        //Reset size of string after pushing back all of the command
        //That is why it doesnt work once you put more than one of the comands
        if (FirstPart.compare("pwd") == 0)
        {
            write(1, "\n", 1);
            printString(dr);//Subject to change size of this
            
        }
        
        //cd command implementation
        //cout << "i is " << i << "\n";
        //cout << "size is " << command.size() << "\n";

        
        else if (FirstPart.compare("cd") == 0)
        {
            //Get the path
            for (int k = i + 1; k < command.size(); k++)
            {
                if (command[k] == ' ')
                {
                    continue;
                }
                else
                {
                    path.push_back(command[k]);
                }
            }
            
            for (int j = 0; j < path.size(); j++)
            {
                cout << path[j];
            }

            if (path == "..")
            {
                path = "";
                for(int j = dr.size() - 1; j >=  0; j--)
                {
                    if (dr[j] == '/')
                    {
                        //cout << "We got a hit and it is " << dr << " at " << j << " \n";
                        for(int k = 0; k < j; k++)
                        {
                            path.push_back(dr[k]);
                        }
                        cout << "Path is " << path << " with length " << path.size() << "\n";
                        break;
                    }
                    else
                    {
                        continue;
                    }

                }

            }//Parent directory
            const char* new_path = path.c_str();
            int newDir = chdir(new_path);
            if (newDir == 0)
            {
               continue;
            }
            else
            {
                continue;
                //ERROR BC IT DOESNT OPEN
            }
            
        }
        
        else if (FirstPart.compare("exit") == 0)
        {
            write(1, "\n", 1);
            exit(1);
        }
        else if (FirstPart.compare("ff") == 0){
            cout << "made it to ff" << "\n";
            // parse other parters of command
            bool has_one_sep_only = false; // only has one separation in command
            bool has_two_sep = false;
            bool previous_space = false;
            string ff_filename; // TODO: allocate memory with 'new'
            string ff_directory;
            cout << "i: " << i << "\n";
            cout << "size of command: " << command.size() << "\n";
            for (int m = i; m < command.size(); m++)
            {
                
                cout << "m: " << m << "\n";
                if (command[m] == ' ')
                {
                    cout << "command at " << m << " is a space\n";
                    cout << "String:" << command << "\n";
                    if (previous_space){
                        continue; // if last character was space also, continue to next character
                    }
                    if (!has_one_sep_only){
                        if (m != command.size() - 1){
                            has_one_sep_only = true;
                            cout << "has one sep" << "\n";
                        }
                    }
                    else if (has_one_sep_only && !has_two_sep){
                        has_one_sep_only = false;
                        if (m != command.size() - 1){
                            has_two_sep = true;
                            cout << "has two sep" << "\n";
                        }
                    }
                    previous_space = true;
                    continue;
                }
                else // non-space character
                {
                    previous_space = false;
                    if (has_one_sep_only){
                        ff_filename.push_back(command[m]);

                    }
                    else if (has_two_sep){ // TODO: add check for more than two parameters (error)
                        ff_directory.push_back(command[m]);
                    }
                }
            }

            const char *ff_dir_char = ff_directory.c_str();
            const char *ff_filename_char = ff_filename.c_str();
            cout << "filename: " << ff_filename << "\n";
            cout << "directory: " << ff_directory << "\n";
            if (has_one_sep_only)
            { 
                // only have filename parameter, use cwd as directory
                ff_recurse(ptr, ff_filename.c_str(), dr, true);
            }
            else if (has_two_sep) 
            {
                cout << "dr: " << dr << "\n";
                ff_recurse(ff_dir_char, ff_filename_char, dr, true);

       //       DIR *ff_dir;
                // struct dirent *ff_struct;
                // string str_current_dir(current_dir);
                // string str_filename(filename);
                // string ff_d_name;
             //     vector<string> ff_vector;
                // if ((ff_dir = opendir(current_dir)) == NULL){
                //     write(1, "ERROR", 5);
                // }
                // else{
                //  while ((ff_struct = readdir(ff_dir)) != NULL){
                //      ff_d_name ff_struct->d_name 
                //      int n = 0;
                //      while (ff_struct->d_name[n] != '\0')
       //                   {
       //                   ff_d_name.push_back(ff_struct->d_name[n]);
       //                   n++;
       //                   }

                //          // use given directory

                            
                //     }
                // }
            }
        }
        else if (FirstPart.compare("ls") == 0)
        {
            cout << "HI\n";
            if (i == command.size())//There is no path
            {
                new_path = ptr;
            }
            else
            {
                for (int l = i + 1; l < command.size(); l++)
                {
                    path.push_back(command[l]);
                }
                new_path = path.c_str();
            }
            
            DIR *ls_directory;
            struct dirent *ls_struct;
            if ((ls_directory = opendir(new_path)) == NULL){
                write(1, "ERROR", 5);
            }
            else{
                while ((ls_struct = readdir(ls_directory)) != NULL){
                    
                    int q = 0;
                    struct stat stats_struct;
                    stat(ls_struct->d_name, &stats_struct);
                    
                    //write("File Permissions: \t");
                    write(1, (S_ISDIR(stats_struct.st_mode)) ? "d" : "-", 1);
                    write(1, (stats_struct.st_mode & S_IRUSR) ? "r" : "-", 1);
                    write(1, (stats_struct.st_mode & S_IWUSR) ? "w" : "-", 1);
                    write(1, (stats_struct.st_mode & S_IXUSR) ? "x" : "-", 1);
                    write(1, (stats_struct.st_mode & S_IRGRP) ? "r" : "-", 1);
                    write(1, (stats_struct.st_mode & S_IWGRP) ? "w" : "-", 1);
                    write(1, (stats_struct.st_mode & S_IXGRP) ? "x" : "-", 1);
                    write(1, (stats_struct.st_mode & S_IROTH) ? "r" : "-", 1);
                    write(1, (stats_struct.st_mode & S_IWOTH) ? "w" : "-", 1);
                    write(1, (stats_struct.st_mode & S_IXOTH) ? "x" : "-", 1);
                    write(1, " ", 1);
                    
                    while (ls_struct->d_name[q] != '\0')
                    {
                        write(1, &ls_struct->d_name[q], 1);
                        q++;
                    }
                    write(1, "\n", 1);
                    
                    
                    //write(1, ls_struct->d_name, 20);
                    //cout << ls_struct->d_name << "\n";
                    //write(1, "\n", 1);
                }
            }
            closedir(ls_directory);
            
        }
        
        
        //size = 0; //Reset the command
        command = "";
        FirstPart = "";
        path = "";
        //Empty out the string again
    }
    
    
  
    
    
    ResetCanonicalMode(0, &Attributes);
    return 0;
    
}
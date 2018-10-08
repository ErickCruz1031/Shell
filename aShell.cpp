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
    bool exit = false;
    char endChar = '%';
    char newLine = '\n';
    const char* new_path;
    
    
    SetNonCanonicalMode(0, &Attributes);
    while (exit != true)
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

        
        //Reset size of string after pushing back all of the command
        //That is why it doesnt work once you put more than one of the comands
        if (FirstPart.compare("pwd") == 0)
        {
            write(1, "\n", 1);
            printString(dr);//Subject to change size of this
            write(1, "\n" , 1);
            
        }
        
        //cd command implementation
        cout << "i is " << i << "\n";
        cout << "size is " << command.size() << "\n";

        
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
            cout << "Path is " << path << " with length " << path.size() << "\n";
            
            for (int j = 0; j < path.size(); j++)
            {
                cout << path[j];
            }
            const char* new_path = path.c_str();
            int newDir = chdir(new_path);
            if (newDir == 0)
            {
                ptr = getcwd(directory, 50);
                write(1, ptr, 50);
                
            }
            //ptr = getcwd(directory, 50);
            
            //cout << "\n";
            
        }
        
        if (FirstPart.compare("ls") == 0)
        {
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


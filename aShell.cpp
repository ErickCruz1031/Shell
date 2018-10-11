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
#include <vector>
#include <signal.h>
#include <sys/wait.h>

using namespace std;

void singleCommand (vector<string> current_command, string wd);
void ResetCanonicalMode(int fd, struct termios *savedattributes){
    tcsetattr(fd, TCSANOW, savedattributes);
}

void outsideCommand(string command, char* const* arguments, char * const environ[])
{
    char cwd[250];
    char* dir = getcwd(cwd, 250);
    char*  env_vars = getenv("PATH");
    int status;
    //char* const environ = env_vars;
    const char *com_str = command.c_str();
    char* p = getenv("PATH");

    pid_t pid = fork(); // 
    if (pid == 0)
    {
        if (execvp(com_str, arguments) == -1)
        {
            cout << "ERROR \n";
        }
        else
        {
            cout << "success\n";
        }
    }
    else
    {
        int child_pid = wait(&status);
        cout << "Finished from " << child_pid << "\n";
    }

    return;

}


bool ff_recurse(const char* current_dir, const char* filename, string last_dir){
    // base case: found a file in directory!
    // recursive case: found another directory, add to path in list of paths
    DIR *ff_dir;
    struct dirent *ff_struct;
    string str_current_dir(current_dir);
    string str_filename(filename);
    string ff_d_name;
    char directory[250];
    string file_path; 

    // cout << "Opening " << str_current_dir << "\n";
    char *ptr = getcwd(directory, 250);
    string str_ptr(ptr);
    //cout << "current: " << str_ptr << " with " << str_current_dir << "\n";

    if ((str_current_dir.compare(".") == 0) || (str_current_dir.compare("..") == 0)){
        return false;
    }
    if (str_current_dir.compare(".git") == 0)
    {
        return false;
    }
    //cout << "Current directory ff: " << str_current_dir << "\n";
    if ((ff_dir = opendir(current_dir)) == NULL)
    {
        //write(1, "ERROR", 5);
        // reached a file-- base case
        if (str_current_dir.compare(str_filename) == 0) // found a match
        {

            //cout << "MATCH \n";
            file_path = str_ptr + "/" + str_filename;
            // print the file

            for (int p = 0; p < file_path.size(); p++){
                write(1, &file_path[p], 1);
            }
            write(1, "\n", 1);

            return true; 
        }
        else{
            return false; // reached non-filename file
        }
    }
    else{ // reached a directory

        string temp;

        //cout << "Opened " << str_current_dir << "\n";
        //cout << "Before " << last_dir << "\n";
        if (last_dir == str_current_dir)
        {
            temp = last_dir;

        }
        else
        {
            temp = last_dir + "/" + str_current_dir;
        }


        //cout << "Just opened "  << temp << "\n";
        //cout << "Other one is " << str_current_dir << "\n";
        chdir(temp.c_str());

        while ((ff_struct = readdir(ff_dir)) != NULL)
        {
            int i = 0;
            while (ff_struct->d_name[i] != '\0')
            {
                ff_d_name.push_back(ff_struct->d_name[i]);
                i++;
            }
            ff_recurse(ff_struct->d_name, filename, temp);
            //cout << "RETURNRED from " << ff_struct->d_name << "\n";
        }
        chdir(last_dir.c_str());
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



int main (int argc, char *argv[], char * const env[])
{
    char buffer;
    string command;
    char directory[255];
    char backup[255];
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
    string current_command;
    vector<vector<string> > input;
    vector<string> temporary;
    int current_index = 0;
    
    //input.resize(1);
    SetNonCanonicalMode(0, &Attributes);
    while (!loop_exit)
    {
        ptr = getcwd(directory, 255);
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
        
        while(read(0, &buffer, 1) > 0)
        {
            //cout << "hi!!!! \n";
        
            if (int(buffer) == 32)
            {
                write(1, &buffer, 1);
                if (!current_command.empty())
                {
                    temporary.push_back(current_command);
                }
                current_command = "";
                continue;
            }

            else if(int(buffer) ==  127)
            {
                //cout << "Delete key!\n";
                write(1, "\b \b", 3);
                continue;
            }
        
           //Implement the backspace
            else if (int(buffer) == 27) //Check to see if it is the arrow
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

            else if (buffer == '\n')
            {
                if (!current_command.empty())
                {
                    temporary.push_back(current_command);

                }

                if (!temporary.empty())
                {
                    input.push_back(temporary);
                }
                break;
            }
            else if (buffer == '|')
            {
                //input.resize(1);
                //input[current_index].push_back(current_command);
                if (!current_command.empty())
                {
                    temporary.push_back(current_command);
                }
                //temporary.push_back(current_command);
                input.push_back(temporary);
                //current_index++;
                temporary.clear();
                current_command = "";
                //current_command = "";

            }
            else
            {
                current_command.push_back(buffer);
            }
            write(1, &buffer, 1);
            //dummy++;
            
        }
        //cout << "The size is " << size << "\n";

        int i = 0;
        cout << "\n" << "Printing...\n";
        for(int i = 0; i < input.size(); i++)
        {
            
            //cout << "Pushing back" << command[i] << "AHHH\n";
            for(int k = 0; k < input[i].size(); k++)
            {
                cout << "'" << input[i][k] << "'";
            }
            cout << "\n";
            
        }//Get length of the command
        cout << current_command << "\n";

            //now all the commands are in input
        cout << "Before it is " << dr << "\n";
        cout << input.size() << "\n";
        if (input.size() == 1)//No piping
        {
            singleCommand(input[0], dr);

        }
        input.clear();

    }



    ResetCanonicalMode(0, &Attributes);
    return 0;
}



void singleCommand (vector<string> current_command, string wd)
{
    cout << current_command[0] << "! !!!!\n";
    cout << wd << "\n";
    if (current_command[0].compare("pwd") == 0)
        {
            cout << "Here" << "\n";
            write(1, "\n", 1);
            printString(wd);//Subject to change size of this
            write(1, "\n", 1);
        }

    else if (current_command[0].compare("cd") == 0)
    {
        //Get the path
        /*
        for (int j = 0; j < path.size(); j++)
        {
            cout << path[j];
        }
        */
        string path;

        if (current_command[1] == "..")
        {
            cout << "It is " << wd << "\n";
            path = "";
            for(int j = wd.size() - 1; j >=  0; j--)
            {
                if (wd[j] == '/')
                {
                    cout << "Found at " << j << "\n";
                    //cout << "We got a hit and it is " << dr << " at " << j << " \n";
                    for(int k = 0; k < j; k++)
                    {
                        path.push_back(wd[k]);
                    }
                    //cout << "Path is " << path << " with length " << path.size() << "\n";
                    break;
                }
                else
                {
                    continue;
                }

            }

        }//Parent directory
        else
        {
            path = current_command[1];
        }
        const char* new_path = path.c_str();
        int newDir = chdir(new_path);
        if (newDir == 0)
        {
            //write(1, "\n", 1); 
            //continue;
            cout << "Opened  \n";
        }
        write(1, "\n", 1); 
    }

    return;
}
/*

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
                cout << "It is " << dr << "\n";
                path = "";
                for(int j = dr.size() - 1; j >=  0; j--)
                {
                    if (dr[j] == '/')
                    {
                        cout << "Found at " << j << "\n";
                        //cout << "We got a hit and it is " << dr << " at " << j << " \n";
                        for(int k = 0; k < j; k++)
                        {
                            path.push_back(dr[k]);
                        }
                        //cout << "Path is " << path << " with length " << path.size() << "\n";
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
                //write(1, "\n", 1); 
                //continue;
                cout << "Opened  \n";
            }
            write(1, "\n", 1); 
        }
        
        else if (FirstPart.compare("exit") == 0)
        {
            write(1, "\n", 1);
            ResetCanonicalMode(0, &Attributes);
            exit(1);
        }
        else if (FirstPart.compare("ff") == 0){

            // cout << "made it to ff" << "\n";
            // parse other parters of command
            bool has_one_sep_only = false; // only has one separation in command
            bool has_two_sep = false;
            bool previous_space = false;
            string ff_filename; // TODO: allocate memory with 'new'
            string ff_directory;
            // cout << "i: " << i << "\n";
            // cout << "size of command: " << command.size() << "\n";

            for (int m = i; m < command.size(); m++)
            {
                
                // cout << "m: " << m << "\n";
                if (command[m] == ' ')
                {
                    continue;
                }
                else // non-space character, start inputting filename
                {
                    cout << "First space at " << m << "\n";
                    int numerator = m;
                    for(; numerator < command.size(); numerator++)
                    {
                        if (command[numerator] == ' ')
                        {
                            break;//End of the filename
                        }
                        else
                        {
                            ff_filename.push_back(command[numerator]);
                        }
                    }//At end of this numerator is at the first space after the filename

                    for (; numerator < command.size(); numerator++)
                    {
                        if (command[numerator] == ' ')
                        {
                            continue;
                        } //Skip any spaces between filename and directory
                        else
                        {
                            for(; numerator < command.size(); numerator++)
                            {
                                ff_directory.push_back(command[numerator]);
                            }
                            break;
                        }
                    }
                    break;

                }
            }

            write(1, "\n", 1);
            
            const char *ff_dir_char = ff_directory.c_str();
            const char *ff_filename_char = ff_filename.c_str();
            cout << "filename: " << ff_filename << "\n";
            cout << "directory: " << ff_directory << "\n";
            //SUbject to change!!!
            //int move = chdir(dr.c_str());//THIS MIGHT NOT BE IT
            int move = chdir(ff_dir_char);
            if (move < 0)
            {
                cout << "NOO\n";
            }
            ff_dir_char = getcwd(backup, 255);
            ff_directory = string(ff_dir_char);
            cout << "Calling... \n";
            ff_recurse(ff_dir_char, ff_filename_char, ff_directory);
            //Move back to our initial directory
            chdir(ptr);

        }
        
        else if (FirstPart.compare("ls") == 0)
        {
            cout << "HI\n";
            if (i == command.size())//There is no path
            {
                new_path = ptr;
                //getcwd
            }
            else
            {
                for (int l = i + 1; l < command.size(); l++)
                {
                    path.push_back(command[l]);
                }
                new_path = path.c_str();
            }
            
            //fork here 
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

        else //outside command + piping
        {
            string new_argument;
            int pickup;
        
            vector<string> arguments;
            cout << "First part is " << FirstPart << "\n";

            arguments.push_back(FirstPart);
            //First part is the 'program'

            cout << "The size is " << command.size() << "\n";

            for(int h = i; h < command.size(); h++)
            {
                //cout << " h is " << h << "\n";
                if (command[h] == ' ')
                {
                    continue;
                }
                else
                {
                    int j = h;
                    for(; j < command.size(); j++)
                    {
                        //cout << "j is " << j << "\n";
                        if (command[j] == ' ')
                        {
                            //cout << "New arg was " << new_argument << "\n";
                            pickup = j;
                            //TBD
                            new_argument.push_back('\0');
                            arguments.push_back(new_argument);
                            break;
                        }
                        else
                        {
                            new_argument.push_back(command[j]);

                        }

                    }

                    if (j == command.size())
                    {
                        new_argument.push_back('\0');
                        arguments.push_back(new_argument);
                        pickup = -1;
                    }


                }
                new_argument = "";
                if (pickup == -1)
                {
                    break;
                }
                else
                {
                    h = pickup;

                }
            }
            //cout << "Here\n";
            
            //cout << "HAHAH\n";
            //cout << "Size before " << arguments.size() << "\n";
            char** args = new char*[arguments.size() + 1];
            //arguments.push_back("\0");
            //cout << "After " << arguments.size() << "\n";


            for(int b = 0; b < arguments.size(); b++)
            {
                args[b] = new char[arguments[b].size()];
                //cout << "b is " << b << "\n";
                strcpy(args[b],arguments[b].c_str());
                cout << "'"<<args[b]<<"'"<< "\n";

            }

            //args[arguments.size()] = new char[1];
            //args[arguments.size()][0] = '\0';


            outsideCommand(FirstPart, args, env);

            //cout << "THE PIPE \n";
        }
        
        
        //size = 0; //Reset the command
        command = "";
        FirstPart = "";
        path = "";
        //Empty out the string again
        //fork, open new file, dup2, call the command
        //call pipe before the fork 
        //if you are a child close down the write of pipe bc you wont need it 
    }
    
    
  
    
    
    ResetCanonicalMode(0, &Attributes);
    return 0;
    
}

*/
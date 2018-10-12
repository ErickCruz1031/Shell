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

bool singleCommand (vector<string> current_command, string wd);
void ResetCanonicalMode(int fd, struct termios *savedattributes){
    tcsetattr(fd, TCSANOW, savedattributes);
}

void outsideCommand(char* const* arguments, int num_args)
{
    int status;
    /*
    cout << "\nPrinting arguments\n";
    for(int i = 0; i < num_args; i++)
    {
        cout << string(arguments[i]);
        cout << "\n";
    }
    */
    //cout << "hello there\n";
    pid_t pid = fork(); // 
    if (pid == 0)
    {
        //cout << "sogknva\n";
        execvp(arguments[0], arguments);
        //cout << "Exec worked\n";
        /*
        if (execvp(arguments[0], arguments) == -1)
        {
            //cout << "ERROR \n";
            //cout << "failed to run "  << "'" << string(arguments[0]) << "'" << "\n";
        }
        else
        {
            //cout << "success\n";
        }
        */
    }
    
    else
    {
        cout << "PARENT\n";
        int child_pid = waitpid(pid, &status, 0);
        //cout << "Status is " << status << " and other is " << child_pid << "\n";
        cout << "Finished from " << child_pid << "\n";
    }
    cout << "RETURN\n";
    

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

    
    //input.resize(1);
    SetNonCanonicalMode(0, &Attributes);
    while (!loop_exit)
    {
        vector<vector<string> > input;
        vector<string> temporary;
        int current_index = 0;
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
        //cout << current_command << "\n";
        

            //now all the commands are in input
        //cout << "Before it is " << dr << "\n";
        //cout << input.size() << "\n";
        if (input.size() == 1)//No piping
        {
            bool done = singleCommand(input[0], dr);
            if (done == true)
            {
                ResetCanonicalMode(0, &Attributes);
                exit(1);
            }

        }
        else
        {
            cout << "\n";
            int fd[2];
            int status;

            pipe(fd);

            pid_t child_one = fork();

            if (child_one == 0)
            {

                cout << "Child with oneee " << input[0][0] << "\n";
                bool inner = false;
                if((input[0][0] == "ls")|| (input[0][0] == "pwd") || (input[0][0] == "ff"))
                {
                    cout << "Inner command!\n";
                    inner = true;
                }

                char** args = new char*[input[0].size() + 1];
                for(int k = 0; k < input[0].size(); k++)
                {
                    args[k] = new char[input[0][k].size()];
                    input[0][k].push_back('\0');
                    strcpy(args[k],input[0][k].c_str());

                }
                args[input[0].size()] = new char[1];
                // args[current_command.size()][0] = '\0';
                args[input[0].size()] = (char*)NULL;


                dup2(fd[1], 1);
                //close(fd[0]);
                //close(fd[1]);
                if (!inner)
                {
                    close(fd[0]);
                    close(fd[1]);
                    execvp(args[0], args);
                }
                else
                {
                    close(fd[0]);
                    singleCommand(input[0], dr);
                    close(fd[1]);
                }
                //execvp(args[0], args);

                //cout << "returned from first\n";

            }

            pid_t child_two = fork();



            if (child_two == 0)
            {
                bool inner_process = false;
                cout << "Child with " << input[1][0] << "\n";
                if((input[1][0] == "ls")|| (input[1][0] == "pwd") || (input[1][0] == "ff"))
                {
                    cout << "Inner command in 2 AHHHHH!\n";
                    inner_process = true;
                }
                char** args = new char*[input[1].size() + 1];

                for(int k = 0; k < input[1].size(); k++)
                {
                    args[k] = new char[input[1][k].size()];
                    input[1][k].push_back('\0');
                    strcpy(args[k],input[1][k].c_str());

                }

                args[input[1].size()] = new char[1];
                // args[current_command.size()][0] = '\0';
                args[input[1].size()] = (char*)NULL;
                //close(fd[1]);
                cout << "About to call\n";
                dup2(fd[0], 0);
                //close(fd[0]);
                //close(fd[1]);
                
                if (!inner_process)
                {
                    close(fd[0]);
                    close(fd[1]);
                    execvp(args[0], args);
                }
                else
                {
                    close(fd[1]);  
                    singleCommand(input[1], dr);
                    close(fd[0]);
                }
                //execvp(args[0], args);
                //close(fd[0]);
                //singleCommand(input[1], dr);
                //close(fd[0]);
                //cout << "Returned from sccnd\n";
            }


            //close(fd[1]);
            close(fd[0]);
            close(fd[1]);
            waitpid(child_one, NULL, 0);
            waitpid(child_two, NULL, 0);
            //if there is a '&' you dont have to wait 

        }
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        input.clear();
        current_command.clear();
        temporary.clear();

    }



    ResetCanonicalMode(0, &Attributes);
    return 0;
}



bool singleCommand (vector<string> current_command, string wd)
{
    //cout << current_command[0] << "! !!!!\n";
    //cout << wd << "\n";
    if (current_command[0].compare("pwd") == 0)
        {
            //cout << "Here" << "\n";
            write(1, "\n", 1);
            printString(wd);//Subject to change size of this
            write(1, "\n", 1);
        }

    else if (current_command[0].compare("cd") == 0)
    {
        //Get the path
        
        string path;

        if (current_command[1] == "..")
        {
            //cout << "It is " << wd << "\n";
            path = "";
            for(int j = wd.size() - 1; j >=  0; j--)
            {
                if (wd[j] == '/')
                {
                    //cout << "Found at " << j << "\n";
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

    else if (current_command[0].compare("exit") == 0)
    {
        write(1, "\n", 1);
        return true;
    }
    
    else if (current_command[0].compare("ff") == 0)
    {


        //string ff_filename; // TODO: allocate memory with 'new'
        string ff_directory;
        write(1, "\n", 1);
        char backup[255];

        
        const char *ff_dir_char = current_command[2].c_str();
        const char *ff_filename_char = current_command[1].c_str();
        cout << "filename: " << current_command[1] << "\n";
        cout << "directory: " << current_command[2] << "\n";
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
        chdir(wd.c_str());

    }
    else if (current_command[0].compare("ls") == 0)
    {
        const char* new_path;
        if (current_command.size() == 1)
        {
            new_path = wd.c_str();
        }
        else
        {
            new_path = current_command[1].c_str();
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

            }
        }
        closedir(ls_directory);
            
    } 
    else //outside command + piping
    {

    
        vector<string> arguments;

        char duh[255];
        char* other = getcwd(duh, 255);

        char** args = new char*[current_command.size() + 1];
        //arguments.push_back("\0");
        //cout << "After " << arguments.size() << "\n";


        for(int b = 0; b < current_command.size(); b++)
        {
            current_command[b].push_back('\0');
            args[b] = new char[current_command[b].size()];
            //cout << "b is " << b << "\n";
            strcpy(args[b],current_command[b].c_str());
            //cout << "'"<<args[b]<<"'"<< "\n";

        }

        args[current_command.size()] = new char[1];
        // args[current_command.size()][0] = '\0';
        args[current_command.size()] = (char*)NULL;

    // if (piping){
    //     execvp(arguments[0], arguments)
    // }
 //else{
        outsideCommand(args, current_command.size());
    // }
        

        //cout << "THE PIPE \n";
    }     

    return false;
}
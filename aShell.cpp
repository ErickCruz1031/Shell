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
#include <deque>

using namespace std;

bool singleCommand (vector<string> current_command, string wd, bool piping);
void ResetCanonicalMode(int fd, struct termios *savedattributes){
    tcsetattr(fd, TCSANOW, savedattributes);
}

void outsideCommand(string command, char* const* arguments, int num_args, bool piping)
{
    int status;

    if (piping){
        cout << "piping in outside command\n";
        execvp(arguments[0], arguments);
    }
    //cout << "hello there\n";
    else{
        pid_t pid = fork(); // 
        if (pid == 0)
        {
            //cout << "sogknva\n";
            execvp(arguments[0], arguments);
            write(1, "Failed to execute ", 18);
            write(1, arguments[0], command.size());
            write(1, "\n", 1);

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
        
        else if (pid > 0)
        {
            // cout << "PARENT\n";
            int child_pid = waitpid(pid, &status, 0);
            //cout << "Status is " << status << " and other is " << child_pid << "\n";
            //cout << "Finished from " << child_pid << "\n";
        }

    }

    // cout << "RETURN\n";
    

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

// function that parses new history str in the same way that main loop parses input
// then push_back to input vector
// continue with read loop

vector<vector<string> > parse_line(string input_string){
    cout << "parsing line: " << input_string << "\n";
    vector<vector<string> > input_vector;
    char curr_char;
    string curr_word;
    vector<string> curr_command;
    for (int i = 0; i < input_string.size(); i++){
        curr_char = input_string[i];
        if (int(curr_char) == 32) // space
        {
            if (!curr_word.empty())
            {
                curr_command.push_back(curr_word);
            }
            curr_word = "";
            continue;
        }
        else if (curr_char == '\n')
        {
            if (!curr_word.empty())
            {
                curr_command.push_back(curr_word);
            }

            if (!curr_command.empty())
            {
                input_vector.push_back(curr_command);
            }
            break;
        }
        else if (curr_char == '|')
        {
            
            if (!curr_word.empty())
            {
                curr_command.push_back(curr_word);
            }
            //temporary.push_back(current_command);
            input_vector.push_back(curr_command);
            //current_index++;
            // TODO: deal with case with two pipes next to each other
                // e.g. Nitta's shell: cat hi.md | | grep -i main fails, prints nothing
            curr_command.clear();
            curr_word = "";

            //current_command = "";

        }
        else{
            curr_word.push_back(curr_char);
        }
    }
    curr_command.push_back(curr_word);
    input_vector.push_back(curr_command);
    return input_vector;

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
    int history_index = 0;
    int input_size = 0;
    int history_size = 0;
    string history_str;
    deque<string> history_queue;
    vector<vector<string> > input_string;
    const char *new_history_char;

    string new_history_str;
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

            // COMMAND HISTORY QUEUE
            
            if (buffer == '\a'){
                history_str.push_back(buffer); // add character to history string
                continue;
            }
            
            // ------------------
            else if (int(buffer) == 32) // space
            {
                history_str.push_back(buffer); // add character to history string
                write(1, &buffer, 1);
                if (!current_command.empty())
                {
                    temporary.push_back(current_command);

                }
                current_command = "";
                continue;
            }

            else if(int(buffer) ==  127) //Implement the backspace
            {
                //cout << "Delete key!\n";
                write(1, "\b \b", 3);
                current_command.pop_back();
                // remove characters from history_str
                if (history_str.size() > 0){
                    history_str.erase(history_str.begin() + history_str.size() - 1);
                }
                continue;

            }
        
            //Check to see if it is the arrow
                     // we always start one past the last element of queue
            // BELL if 
                // history_index == 0 and UP pressed 
                // history_index == history_size and DOWN pressed
            // special cases
                // history_index == history_size - 1 and DOWN --> show an empty line
                // history_index == history_size and UP --> clear string read and show empty if returned here
            // else
                // UP --> history_index-- and show history_queue[history_index]
                // DOWN --> history_index++ and show history_queue[history_index]
                // then 

            // Nitta's example command history special case:
            // Implement/ ask about this later.
                // 1)  UP --> history_index --, show history[history_index]
                // 2) continue to edit starting with above text ^
                // 3) '\n' --> output appears
                // 4) UP again --> shows the same history[history_index] as before ^
                // 5) DOWN --> shows the editted command


            else if (int(buffer) == 27) // ESCAPE
            {
                cout << "size of history queue is " << history_size << "\n";
                read(0, &buffer, 1); 
                if (int(buffer) == 91) // [
                {


                    read(0, &buffer, 1);               
                    if(int(buffer) == 65) // A
                    { // UP ARROW 
                        

                        // ca
                        new_history_str = history_str;
                        if (history_index == 0){
                            // BELL
                            
                            write(1, "\a", 1);
                        }
                        else{
                            temporary.clear();
                            history_str.clear(); 

                            //if (history_index == history_size){
                            // clear and move up
                                // history_index--;
                            
                                // for (int n = 0; n < history_str.size(); n++){
                                //     write(1, "\b", 1);
                                // }

                                
                                // cout << "cleared history_str\n";
                           // }
                        
                            // else{
                          //     // delete current entry 
                            //     history_index--;
                            // }
                        

                            history_index--;
                            cout << "decremented index to " << history_index << "\n";
                            for (int n = 0; n <new_history_str.size(); n++){
                                write(1, "\b", 1);
                            } // clear 
                            // cout << "deleted current input\n";

                            // show previous entry
                            new_history_str = history_queue[history_index]; // shallow copy?

                            cout << "new history str is " << new_history_str << "\n";
                            cout << "size of new history str " << new_history_str.size() << "\n";
                            new_history_char = new_history_str.c_str();

                            for (int j = 0; j < new_history_str.size(); j++){
                                write(1, &new_history_char[j], 1);
                                history_str.push_back(new_history_str[j]);
                            }
                            new_history_char = "";
                            // need to push every space-separated item in new history str into temporary

                            // current_command = new_history_str; 
                            input = parse_line(new_history_str);
                            cout << "input after parsing line: \n";
                            for (int input_index = 0; input_index < input.size(); input_index++){
                                for (int inside_index = 0; inside_index < input[input_index].size(); inside_index++){
                                    cout << input[input_index][inside_index] << "\n";
                                }
                                
                            }

                            //TENTATIVE
                           // TODO: Somehow continue parsing into input vector<vector<string>>
                            // Problem: can't write to actual stdin in such a way that program actually 
                                // reads from it (would need redirection)

                            // cout << "finished writing after UP\n";

                        }

                        
                    }
                    else if (int(buffer) == 66) //
                    {
                        // DOWN ARROW 
                        cout << "DOWN\n";
                        new_history_str = history_str;
                        if (history_index == history_size){
                            // BELL
                            cout << "BELL\n";
                            write(1, "\a", 1);
                        }
                        else{
                            temporary.clear(); // TENTATIVE
                            history_str.clear(); 
                            history_index++;
                            cout << "incremented index to " << history_index << "\n";
                        // remove previous output from screen
                            for (int p = 0; p < new_history_str.size(); p++){
                                write(1, "\b", 1);
                            }
                            cout << "size of queue: " << history_size << "\n";
                            new_history_char = new_history_str.c_str();
                            if (history_index < history_size){
                                // don't want to access history_queue[history_size] --> segfault
                                new_history_str = history_queue[history_index];

                                cout << "new history str is " << new_history_str << "\n";
                                cout << "size of new history str " << new_history_str.size() << "\n";
                                for (int q = 0; q < new_history_str.size(); q++){
                                    write(1, &new_history_char[q], 1);
                                    history_str.push_back(new_history_str[q]);
                                }
                            }
                            else{
                                // TODO

                            }
                            new_history_char = "";
                            current_command = new_history_str; //TENTATIVE
                            
                        }
                        

                    
                    }
                }
            }

            else if (buffer == '\n')
            {
                write(1, &buffer, 1);
                history_queue.push_back(history_str); // add string to queue
                cout << "added " << history_str << " to queue\n";
                history_size++; // increment size of queue
                history_index = history_size;
                if (history_queue.size() > 10){
                    history_queue.pop_front(); // if there are more than 10 commands, get rid of one
                    history_size--; 
                }
                history_str.clear();
                new_history_str.clear();

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
                write(1, "|", 1);
                history_str.push_back(buffer); // add character to history string
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
                write(1, &buffer, 1);
                history_str.push_back(buffer); // add character to history string
                current_command.push_back(buffer);
            }
            
            //dummy++;
            
        }
        //cout << "The size is " << size << "\n";

        int i = 0;
        cout << "\n" << "Printing...\n";
        cout << "size of input: " << input.size() << "\n";
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
        if (input.size() == 0){
            cout << "do nothing\n";
        }
        else if (input.size() == 1)//No piping
        {
            cout << "going into single command with " << input[0][0] << "\n";
            bool done = singleCommand(input[0], dr, false);
            if (done == true)
            {
                ResetCanonicalMode(0, &Attributes);
                exit(1);
            }

        }
        else //piping
        {
            int fd[2];
            int status;

            pipe(fd);


            pid_t pid = fork();

            if (pid == 0)
            {
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                // cout << "executing first\n";
                singleCommand(input[0], dr, true);

                cout << "returned from first\n";

            }
            else{
                pid_t pid2 = fork();
                // dup2(fd[0], 0);
                // close(fd[0]);
                // close(fd[1]);
                // waitpid(pid, &status, 0);
               
                
                if (pid2 == 0){
                
                    dup2(fd[0], 0);
                    close(fd[0]);
                    close(fd[1]);
                    singleCommand(input[1], dr, true);
                    
                    // cout << "Returned from sccnd\n";
                }
                else if (pid2 > 0){
                    
                    close(fd[0]);
                    close(fd[1]);
                    waitpid(pid2, &status, 0);
                }

            }
        


            //close(fd[1]);
            // close(fd[0]);
            // waitpid(child_one, NULL, 0);
            // waitpid(child_two, NULL, 0);
            //if there is a '&' you dont have to wait 

        }
        input.clear();
        
        temporary.clear();
        current_command = "";
    }
        
    ResetCanonicalMode(0, &Attributes);
    return 0;
}

// parse_cmd(vector<string> current_command, char& )

bool singleCommand (vector<string> current_command, string wd, bool piping)
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

        if (current_command.size() == 1){
            const char* home = getenv("HOME");
            chdir(home);
        }

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

        if (newDir == -1){
            write(1, "Error changing directory.", 26);
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
        // cout << "filename: " << current_command[1] << "\n";
        // cout << "directory: " << current_command[2] << "\n";
        //SUbject to change!!!
        //int move = chdir(dr.c_str());//THIS MIGHT NOT BE IT
        int move = chdir(ff_dir_char);
        if (move < 0)
        {
            // cout << "NOO\n";
        }
        ff_dir_char = getcwd(backup, 255);
        ff_directory = string(ff_dir_char);
        // cout << "Calling... \n";
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
        cout << "going into outside command with " << current_command[0] << "\n";
        outsideCommand(current_command[0], args, current_command.size(), piping);
    // }
        

        //cout << "THE PIPE \n";
    }     

    return false;
}
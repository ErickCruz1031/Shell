/*
//////////////////

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
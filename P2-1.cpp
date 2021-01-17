/*
Chris DeStefano
CIS - 3207 Sec 003
Dr. Tamer Aldwairi 
10-16-2019
Project 2 
*/
#include <fstream>
#include <iostream> 
#include <string> 
#include <cstring>  
#include <iomanip> 
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <vector>
#include <stdio.h> 
#include <string.h> 
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>






using namespace std;

#define SHELLSIZE 35



struct command
{
string cmd;
string cmd_arg;
string exe;
string exe_arg1;
string exe_arg2; 
string exe2;
string exe2_arg1;
string exe2_arg2; 
bool background; 
int out; // 0 = DNE | 1 = > | 2 = >>
int inp; // 0 = DNE | 1 = > 
string infile; 
string infi_arg1; 
string infi_arg2; 
string outfile;
string outfi_arg2;
string outfi_arg1;
bool pipe; 
string pipe_in; 
string pipe_out; 
string pin_arg1;
string pin_arg2;
string pout_arg1;
string pout_arg2; 
};

string env_var =""; 
char error_message[30] = "An error has occurred\n";

void init_struct (command& newcmd){
    newcmd.cmd = ""; 
    newcmd.cmd_arg = "";
    newcmd.exe = ""; 
    newcmd.exe2 = "";
    newcmd.exe_arg1 = "";
    newcmd.exe_arg2 = "";
    newcmd.exe2_arg1 = "";
    newcmd.exe2_arg2 = "";
    newcmd.background = false;  
    newcmd.infile = "";
    newcmd.infi_arg1 ="";
    newcmd.infi_arg2 ="";
    newcmd.inp = 0;
    newcmd.out= 0;
    newcmd.outfile = "";
    newcmd.pipe = false; 
    newcmd.pipe_in=""; 
    newcmd.pipe_out=""; 
    newcmd.outfi_arg2="";
    newcmd.outfi_arg1="";

 
    
}






/*
Is passed the current path and prints all the elements in the current directory
*/
void ls (string path){


      const char* PATH = path.c_str(); //sets path to the current path

    DIR *dir = opendir(PATH); //creates a ptr to direct stream and sets it to the path 

    struct dirent *entry = readdir(dir); //creates a struct of type dirent

    while (entry != NULL) //keeps reading entry's from dir till it returns null
    {
        if (entry->d_type == DT_DIR || entry->d_type == DT_REG )
            //printf("%s\n", entry->d_name);
            cout << entry->d_name << endl; 
       
        entry = readdir(dir);
    }

    closedir(dir);

 
}
/*
Modified version of ls that returns a vector of the content to use for comparing
*/
vector<string> get_ls (){

int buffsize = 100000; 
char buff[buffsize];
vector<string> content;
string cur_Path = getcwd(buff,buffsize);
const char* PATH = cur_Path.c_str();//sets path to the current path
DIR *dir = opendir(PATH); //creates a ptr to direct stream and sets it to the path 
struct dirent *entry = readdir(dir); //creates a struct of type dirent

    while (entry != NULL) //keeps reading entry's from dir till it returns null
    {
        if (entry->d_type == DT_REG )

            content.push_back(entry->d_name); 
        entry = readdir(dir);
    }

    closedir(dir);

return content; 
}








/*
prints new lines = to shell size; 
*/
void clear_scrn(){
for (int i =0; i<=SHELLSIZE; i++)
    cout << "\n";
}


/*
Changes the current directory to string path
Returns true if it was successful 
*/
int chng_dir(string path){

int er =0;
int buffsize = 1000; 
char buff[buffsize];
const char * p = path.c_str(); 
char * cur_path = getcwd(buff,buffsize); 
char * home_path = getenv("HOME");
string cur_path_string(cur_path); 
string home_path_string(home_path); 
string rel_path = "";
//cout << home_path_string << endl; 

string check_4_home = ""; 

for (int i=0; i <= home_path_string.size(); i++){
    check_4_home = p[i];
   // cout << check_4_home;  
}


if (path.find(home_path_string) != string::npos ){
    er = chdir(p);
    return er; 
}

    cur_path_string += path; 
    const char * new_p = cur_path_string.c_str();
    er = chdir(new_p);
 







return er; 
}


/*
Print envrioment variables:
Current user
User’s home path
Shell name
OS type
Hostname
*/
void print_enviroment (){


cout << "SHELL_NAME=MyShell\n"; 
cout << "OPERATING_SYSTEM=Linux\n";
cout << "USER=" << getenv("USER") << endl;
cout << "HOME=" << getenv("HOME") << endl;
cout << "NAME=" << getenv("NAME") << endl;
cout << "HOSTTYPE=" << getenv("HOSTTYPE") << endl;


}

int countOccurence(string &patern, string &input){

    int n = patern.length(); 
    int k = input.length(); 
    int occurences =0; 

    for (int i = 0; i <= k - n; i++){  
        int j; 
        for (j = 0; j < n; j++) 
            if (input[i+j] != patern[j]) 
            break; 
        if (j == n) { 
           occurences++; 
           j = 0; 
        } 
    } 
 
    return occurences;
}



/*
Takes the command entered by the user as an input 
Returns the command struct 
*/
command get_command(string input){
string pat = " & ";

int h = countOccurence(pat,input); 

int h_it =0; 
command in;
vector <string> tok; 
stringstream c1(input); 
string temp; 
tok.clear(); 
tok.resize(0); 


    while(getline(c1, temp, ' ')) 
    { 
        tok.push_back(temp); 
    } 


//arg 1 will always be a command or file name - Handled 
//arg 2 will either be an argument or redirection operator or a pipe 
    //If arg 2 is a redirection operator then arg 3 will be a file or program 
    
//Arg 3 will either be an argument or file or redirection operator or a pipe 
    //Arg 3 will be an argument if it exists and is not equal to a redirection operator
    //Arg 3 will only be a file name if arg2 is a redirection operator 
//Arg 4 will either be redirection or file name or a pipe 
    //Arg 4 will be redirection if arg 3 is a file name
    //
//Arg 5 will either be redirection or file name 
//Arg 6 will either be redirection or file name 
//Arg 7 will either be redirection or file name 
// if arg 2 is a redirection operator then arg 3 is a file 
/*

    //if tok[1] exists and is not a redirection operator then it must be an argument 
    if (tok.size() >= 2 && tok[1].compare(">") != 0 && tok[1].compare("<") != 0 && tok[1].compare(">>") != 0 && tok[1].compare("<<") != 0){
        in.arg1 = tok[1];
        in.output = 0; 
        in.input = 0; 
       // cout << "arg1 = " << in.arg1 << "\n"; 
    }
   // if tok[1] exists and is a redirection operator


each part of the input could either be
A command (can check for internal commands)
a filename or exe (can check for one of the files in the list returnd by get_ls)
an arg 
 |
 >
 <
 >>

 


*/
/*
for(int i=0; i<=tok.size(); i++){
    cout << tok[i] << endl; 
}
*/


init_struct(in); 
vector<string> contents; 
contents = get_ls(); // list of all the files in the cur dir 
string cmds[8] = {"quit","clr","pause","help","echo","environ","dir","cd"};
int count = 0;
int a =0; // 0 means i has not been assigned, when a = 1 i has been assigned 
//checking if a output redirector exists for rediring an interal command 
int o = 0; 
    //
   /*
   if(tok.size()>=3){ // tok's size must be at least three to be using an output operator 
        for (int i=0; i<=tok.size(); i++){
            if(tok[i].compare(">") == 0) {

                o = 1;
            } 
            else if (tok[i].compare(">>") == 0){
                o = 2; 
            }
                
        }
   }
   */
   
  
size_t found_out = found_out=input.find(">");
    if (found_out != string::npos){
         o = 1;
    }
found_out = found_out=input.find(">>");
    if (found_out != string::npos){
            o = 2; 
       
       }
      

    



     
    for (int i=0; i<=tok.size(); i++){ //for loop to cycle elements of tok
     
        //cout << tok[i] << endl;
        count = i+1; 
        a=0; 
            
    if(tok.size()>=count){
       while(a == 0){
         if(tok[i].compare("")==0){
            a =1; 
         }  

        for(int j=0; j<=7;j++){ //checks for intenral commands 
         
            string comp = cmds[j];
                
                if(tok[i].compare(comp) == 0){ 
                    
                    in.cmd = tok[i]; 
                    
                     //error with finding the args 
                        if(in.cmd.compare("echo")==0){ // if its one of the int cmds with args
                            in.cmd_arg = tok[i+1];  // then the following input must be its arg 
                            tok[i+1].erase();
                            if (o==1){
                                in.out = 1;
                            }
                            else if (o==2){
                                in.out =2; 
                            }
                        }
                        
                        if(in.cmd.compare("cd")==0){
                            
                            in.cmd_arg = tok[i+1];
                            tok[i+1].erase();
                            tok.resize(tok.size()-1); 
                        }
                        if (in.cmd.compare("dir")==0 && (o ==1 || o ==2) ){
                            if (o==1){
                                in.out = 1;
                            }
                            else if (o==2){
                                in.out =2; 
                            }
                        }
                        if (in.cmd.compare("environ")==0 && (o ==1 || o ==2) ){
                            if (o==1){
                                in.out = 1;
                            }
                            else if (o==2){
                                in.out =2; 
                            }
                        }
                        if (in.cmd.compare("help")==0 && (o ==1 || o ==2) ){
                            if (o==1){
                                in.out = 1;
                            }
                            else if (o==2){
                                in.out =2; 
                            }
                        }
                        
                       a=1; 
                }
                
        }// ends checking for internal cmds and args
         
        if (i>0){
            if(tok[i-1].compare("echo") ==0){ //echo can only be called with an arg 
                a = 1;
            }
        }
        for (int k =0; k<=contents.size(); k++){ //checks for file or executable in cur_dir
           
            string comp2 = contents[k];
            //cout << comp2 << endl;  
                if (tok[i].compare(comp2) == 0){
                    
                    if (comp2.find(".txt") != string::npos){
                        
                        if (i > 0 &&tok[i-1].compare("<") == 0){ //if the ele before i is redir
                            in.infile = tok[i]; 
                            a=1;
                        }
                        else if(i > 0 && (tok[i-1].compare(">") == 0 || tok[i-1].compare(">>") == 0)){
                            in.outfile = tok[i]; 
                            a=1;
                        }
                        else if (i > 0 &&tok[i-1].compare("|") == 0){
                            in.pipe_out = tok[i]; 
                            a=1;
                        }
                        else if (i > 0 &&in.infi_arg1.compare("")==0 &&tok[i-1].compare(in.infile) ==0){
                            in.infi_arg1 = tok[i]; 
                            a=1;         
                        }
                        else if (i > 1 &&in.infi_arg2.compare("")==0 &&tok[i-2].compare(in.infile) ==0&& tok[i-1].compare("<") != 0 && tok[i-1].compare(">") != 0 && tok[i-1].compare(">>") != 0 && tok[i-1].compare("|") != 0){
                            in.infi_arg2 = tok[i]; 
                            a=1;
                        }
                        else if (i > 1 &&in.outfi_arg1.compare("")==0 &&tok[i-2].compare(in.outfile) ==0&& tok[i-1].compare("<") != 0 && tok[i-1].compare(">") != 0 && tok[i-1].compare(">>") != 0 && tok[i-1].compare("|") != 0){
                            in.outfi_arg1 = tok[i]; 
                            a=1;
                        }
                        else if (i > 0 &&in.outfi_arg2.compare("")==0 &&tok[i-1].compare(in.outfile) ==0){
                            in.outfi_arg2 = tok[i]; 
                            a=1;
                        }
                        else if (i > 0 && in.pin_arg1.compare("")==0 && tok[i-1].compare(in.pipe_in)){
                            in.pin_arg1 = tok[i];
                            a=1;
                        }
                        else if (i > 1 && in.pin_arg2.compare("")==0 && tok[i-2].compare(in.pipe_in)&& tok[i-1].compare("<") != 0 && tok[i-1].compare(">") != 0 && tok[i-1].compare(">>") != 0 && tok[i-1].compare("|") != 0){
                            in.pin_arg2 = tok[i];
                            a=1;
                        }
                        else if (i > 0 &&in.exe_arg1.compare("")==0 &&tok[i-1].compare(in.exe)){
                            in.exe_arg1 = tok[i];
                            a=1;
                        }
                        else if (i > 1 && in.exe_arg2.compare("")==0 && tok[i-2].compare(in.exe) && tok[i-1].compare("<") != 0 && tok[i-1].compare(">") != 0 && tok[i-1].compare(">>") != 0 && tok[i-1].compare("|") != 0){
                            in.exe_arg2 = tok[i];
                            a=1;
                        }
                    }//end checking .txt

                    else { //if the file doesent not contain .txt then it must be an exe
                      // segmentation fault somewhere in this
                        if (tok.size() > 1 && i > 1){
                            if (in.infile.compare("")==0 &&tok[i-1].compare("<") == 0){ //if the ele before i is redir
                                in.infile = tok[i];
                                a=1; 
                            }
                            else if (in.outfile.compare("")==0 &&tok[i-1].compare(">") == 0 || tok[i-1].compare(">>") == 0){
                                in.outfile = tok[i]; 
                                a=1;
                            }
                            else if (in.pipe_out.compare("")==0 &&tok[i-1].compare("|") == 0){
                                in.pipe_out = tok[i]; 
                                a=1;
                            }
                        }
                        else if(i >=0 && tok.size()>=3 && a ==0){
                        
                            if (i+1 <= tok.size()&&in.pipe_in.compare("")==0 &&tok[i+1].compare("|")==0){
                                in.pipe_in = tok[i];
                                a=1; 
                            }
                        
                        
                            else if (i+2 <= tok.size()&&in.pipe_in.compare("")==0 &&tok[i+2].compare("|")==0){
                                in.pipe_in = tok[i]; 
                                a=1;
                            }
                        
                        
                           else  if (i+3 <= tok.size()&&in.pipe_in.compare("")==0 &&tok[i+3].compare("|")==0 && tok[i+2].compare(">") != 0&& tok[i+2].compare("<") != 0 && tok[i+2].compare(">>") !=0){
                                in.pipe_in = tok[i]; 
                                a=1;
                            }
                           
                        }
                        if (i == 0){
                           in.exe = tok[i];  
                           a=1;
                        }
                        
                        
                    }//end checking exe
                
            }//end if match    
        } 
    
        
        if (tok[i].compare(">") == 0){
                in.out = 1; 
                a=1;
        }
        else if (tok[i].compare(">>") == 0){
                in.out = 2;
                a=1;
        }
        else if (tok[i].compare("<") == 0){
                in.inp = 1; 
                a=1;
        }
        else if (tok[i].compare("|") == 0){
                in.pipe = true; 
                a=1;
        }
        else if (tok[i].compare("&")==0){
            in.background=true; 
            a=1;
        }
        
        // If the ele is not a An internal command, or one if its args, a file in the cur dir 
        //  or a redirection/background symbol then it must be an arg to either an exe, a in/out pipe,
        // or the in/out redir file 
        
        else { 
                if (in.exe_arg1.compare("")==0 &&i > 0 &&tok[i-1].compare(in.exe)==0){
                    in.exe_arg1 = tok[i];
                    a=1;
                }  
                else if (in.exe_arg2.compare("")==0 && i > 1 &&tok[i-2].compare(in.exe)==0&& tok[i-1].compare("<") != 0 && tok[i-1].compare(">") != 0 && tok[i-1].compare(">>") != 0 && tok[i-1].compare("|") != 0){
                    in.exe_arg2 = tok[i];
                    a=1;
                }  
                else if (in.pin_arg1.compare("")==0 && i > 0 &&tok[i-1].compare(in.pipe_in)==0){
                    in.pin_arg1 = tok[i];
                    a=1;
                }  
                else if (i > 1 && in.pin_arg2.compare("")==0 &&tok[i-2].compare(in.pipe_in)==0&& tok[i-1].compare("<") != 0 && tok[i-1].compare(">") != 0 && tok[i-1].compare(">>") != 0 && tok[i-1].compare("|") != 0){
                    in.pin_arg2 = tok[i];
                    a=1;
                }  
                else if (i > 0 &&in.pout_arg1.compare("")==0 &&tok[i-1].compare(in.pipe_out)==0){
                    in.pout_arg1 = tok[i];
                    a=1;
                }  
                else if (i > 1 &&in.pout_arg2.compare("")==0 &&tok[i-2].compare(in.pipe_out)==0&& tok[i-1].compare("<") != 0 && tok[i-1].compare(">") != 0 && tok[i-1].compare(">>") != 0 && tok[i-1].compare("|") != 0){
                    in.pout_arg2 = tok[i];
                    a=1;
                }
                else if (i > 0 &&in.infi_arg1.compare("")==0 &&tok[i-1].compare(in.infile) ==0){
                    in.infi_arg1 = tok[i];
                    a=1;
                }  
                else if (i > 1 &&in.infi_arg2.compare("")==0 &&tok[i-2].compare(in.infile)==0&& tok[i-1].compare("<") != 0 && tok[i-1].compare(">") != 0 && tok[i-1].compare(">>") != 0 && tok[i-1].compare("|") != 0){
                    in.infi_arg2 = tok[i];
                    a=1;
                }
                else if (i > 0 &&in.outfi_arg1.compare("")==0 &&tok[i-1].compare(in.outfile) ==0){
                    in.outfi_arg1 = tok[i];
                    a=1;
                }  
                else if (i > 1 &&in.outfi_arg2.compare("")==0 &&tok[i-2].compare(in.outfile)==0&& tok[i-1].compare("<") != 0 && tok[i-1].compare(">") != 0 && tok[i-1].compare(">>") != 0 && tok[i-1].compare("|") != 0){
                    in.outfi_arg2 = tok[i];
                    a=1;
                } 
                if (a==0){ // if a still 0 after the whole conditional then it must be a false input 
                a = 1; 
                }
           }
           
         }// ends checking if i has been assigned yet      
   
 


 

}// end if check size
}// end for loop

return in; 
}
vector<command> get_para_command(string input){

string par = " & ";
int num_cmd = 1; 
num_cmd = countOccurence(par, input) +1;


vector <command> parallel; 
parallel.resize(num_cmd);
string temp;

char * cstr_input = new char [input.length()+1];
strcpy (cstr_input, input.c_str());
char * token; 
char * rem = cstr_input; 
int count = 0; 
int k = 0; 

vector <string> para_commands; 


    while(token= strtok_r(rem, "&", &rem)){
        para_commands.push_back(token); 
            if (para_commands[k].at(0) == ' '){ // if the command has white space at the begging remove it 
                para_commands[k].erase(0,1); 
            }
        //cout << para_commands[k] << endl; 
        k++;
         
    }


    while (count != num_cmd){
        parallel[count]=get_command(para_commands[count]);  
        count++;
    }



    return parallel;
}


void run_command(string user_input){
    int buffsize = 100000; 
    char buff[buffsize];
    int stdout_backup = dup(1);
    int stdin_backup = dup(0);
    string pattern = " & ";
    string cur_Path = getcwd(buff,buffsize); 
    command input;
    vector <command> para_cmd;
    env_var=cur_Path;
   
   if (countOccurence(pattern, user_input)==0){
        

        input = get_command(user_input); 
        if(input.background){
            close(0);
            close(1);
        }


         // if cmd = quit 
        if (input.cmd.compare("quit") == 0){
            exit(0);  
        }
           // if cmd = clr 
        else if (input.cmd.compare("clr") == 0){
            clear_scrn();
        }
           // if cmd = cd 
        else if (input.cmd.compare("cd") == 0){
           if(chng_dir(input.cmd_arg) == 0) // if chng_dir returns true 
                cur_Path = getcwd(buff,buffsize); //set the current path to be displayed wit the current path
            else
            {
                cout << input.cmd_arg << " is not a valid path \n";            }
            
        }
               // if cmd = dir 
        else if (input.cmd.compare("dir") == 0&& input.out == 0) {
            ls(cur_Path);
        }
           // if cmd = environ
        else if(input.cmd.compare("environ")==0&& input.out == 0){
            print_enviroment(); 
        }
               // if cmd = echo 
        else if (input.cmd.compare("echo")==0&& input.out == 0){
            cout << input.cmd_arg << endl; 
        }
           // if cmd = pause 
        else if (input.cmd.compare("pause")==0){
            
            do {
                cout << "press enter to continue"; 
            }while(cin.get() != '\n');
        
        }
           // if cmd = help 
        else if (input.cmd.compare("help")==0 && input.out == 0){
            cur_Path = getcwd(buff,buffsize);
            string shell_path = ""; 
            shell_path = env_var; 
            if(chng_dir(env_var) == 0){
                ifstream File; 
                File.open("readme");
                    if (!File){
                        cout << "Can't open file" << endl; 
                        
                    }
                
                cout << File.rdbuf();
                chng_dir(cur_Path); 
                File.close();
            }   
            else {
                cout << "error in help\n"; 
            }
                
                
        } //if we are stricly executing an exe with no redirection 
        else if (input.exe.compare("") != 0 && !input.pipe && input.out == 0 && input.inp == 0){ // if exe is not null 
            
            
            pid_t ChildPID; 
            string enviroment = cur_Path; 
            ChildPID= fork(); 
            int er = 0; 
            char * argv[2]; 
            string file= cur_Path+"/"+input.exe;
            char * cstr = new char [file.length()+1];
            strcpy(cstr, file.c_str());
           
            
            //If exe's args are not null then add them to the list of args to pass
            if (input.exe_arg1.compare("") != 0){
                char * temp = new char [input.exe_arg1.length()+1];
                strcpy(temp, input.exe_arg1.c_str());
                argv[0] = temp; 
            }
            if (input.exe_arg2.compare("") != 0){
                 char * temp = new char [input.exe_arg2.length()+1];
                strcpy(temp, input.exe_arg2.c_str());
                argv[1] = temp;
            }


            //executing the program 
            if (ChildPID >= 0){  // fork was successful 
                if (ChildPID == 0){ //child process 
                  er= execv(cstr, argv);
                   
                
                }
                else { //parent process 
                    wait(0); 
                }
            }
            else{//fork failed
                perror("fork failed");
            }  
    
        }
        // if we are just redirection input 
        else if (input.inp == 1 && input.out == 0){
            int file2; 
            file2 = open(input.infile.c_str(), O_RDONLY ); 
            int backup=dup(0); 
                     

                pid_t ChildPID; 
                string enviroment = cur_Path; 
                int er = 0; 
                char * argv[2]; 
                string file= cur_Path+"/"+input.exe;
                char * cstr = new char [file.length()+1];
                strcpy(cstr, file.c_str());
            
                    
                 //If exe's args are not null then add them to the list of args to pass
                    if (input.exe_arg1.compare("") != 0){
                            char * temp = new char [input.exe_arg1.length()+1];
                            strcpy(temp, input.exe_arg1.c_str());
                            argv[0] = temp; 
                    }
                    if (input.exe_arg2.compare("") != 0){
                            char * temp = new char [input.exe_arg2.length()+1];
                            strcpy(temp, input.exe_arg2.c_str());
                            argv[1] = temp;
                    }
                        
                        ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(0); 
                            dup2(file2,0);
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  
            
                        
                        
                        
                        
                 dup2(0,file2); //reset the stream 
                 close(file2); 


        }// end stricly file input redirection 
        // if we are just redirecting output > means create and truncate
        else if ((input.out == 1 || input.out == 2) && input.inp == 0){
            int buffsize1 = 100000; 
            char buff1[buffsize1];
            fstream file;
            int file2;  
            streambuf* stream_buffer_cout = cout.rdbuf(); //backing up cout streambuffer
            int out_type = 0; // 1=exe , 2 = interal command 
                
                if (input.cmd.compare("")!=0){
                    out_type = 2;
                }
                else {
                    out_type = 1; 
                }

            
                
            
                if (out_type == 2){ //handle internal command first 
                        if (input.out == 1 ){ // if out = 1 we truncate file 
                                file.open(input.outfile, fstream::trunc | fstream::out); //open the file for truncating and set the stream
                        }
                        else if (input.out == 2){// if out = 2 we append file 
                                file.open(input.outfile, fstream::app | fstream::out);
                        }
                        
                    streambuf* strm_buff_file = file.rdbuf(); //get file stream buff
                    cout.rdbuf(strm_buff_file); //redriect cout to the file stream buff 
                        if (input.cmd.compare("help")==0){

                            cur_Path = getcwd(buff1,buffsize1);
                            string shell_path = ""; 
                            shell_path = env_var; 
                                if(chng_dir(env_var) == 0){
                                    ifstream Fi; 
                                    Fi.open("readme");
                                        if (!Fi){
                                            cout << "Can't open file" << endl; 
                                                
                                        }
                                        
                                    cout << Fi.rdbuf();
                                    chng_dir(cur_Path); 
                                    Fi.close();
                                    }   
                                    else {
                                        cout << "error in help\n"; 
                                    }
                        }///ends help 
                    else if(input.cmd.compare("dir")==0){
                        ls(cur_Path);
                    }
                    else if (input.cmd.compare("echo")==0){
                        cout << input.cmd_arg << endl; 
                    }
                    else if (input.cmd.compare("environ")==0){
                        print_enviroment(); 
                    }

                    cout.rdbuf(stream_buffer_cout); //resets the stream buffer back to cout 
                    file.close(); 
                 }
                else if (out_type == 1){ //handles executable writing to an out file 
                    
                    if (input.out == 1 ){ // if out = 1 we truncate file 
                                file2=open(input.outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT); //open the file for truncating and set the stream
                    }
                    else if (input.out == 2){// if out = 2 we append file 
                                file2=open(input.outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                    }

                    
                    int backup=dup(1); 
                     

                    pid_t ChildPID; 
                    string enviroment = cur_Path; 
                    int er = 0; 
                    char * argv[2]; 
                    string file= cur_Path+"/"+input.exe;
                    char * cstr = new char [file.length()+1];
                    strcpy(cstr, file.c_str());
                
                    
                    //If exe's args are not null then add them to the list of args to pass
                        if (input.exe_arg1.compare("") != 0){
                            char * temp = new char [input.exe_arg1.length()+1];
                            strcpy(temp, input.exe_arg1.c_str());
                            argv[0] = temp; 
                        }
                        if (input.exe_arg2.compare("") != 0){
                            char * temp = new char [input.exe_arg2.length()+1];
                            strcpy(temp, input.exe_arg2.c_str());
                            argv[1] = temp;
                        }

                        ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); 
                            dup2(file2,1);
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  
            
                        
                        
                        
                        
                 dup2(1,file2); //reset the stream  
                 
                close(file2);   
                
                }// ends redricting exe



                
                
        }
        //if we are redirecting both input and output IE EXE is taking its input from 
        // input.infile and its output is being redirected to input.outfile
        else if ((input.out == 1 || input.out == 2) && input.inp == 1){
            int file2;
            int file = open(input.infile.c_str(), O_RDONLY); 
            int in_back = dup(0);
            int out_back = dup(1); 
            pid_t ChildPID; 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv[2]; 
            string loc= cur_Path+"/"+input.exe;
            char * cstr = new char [loc.length()+1];
            strcpy(cstr, loc.c_str());

                   
                   
                   
                if (input.out == 1 ){ // if out = 1 we truncate file 
                                file2=open(input.outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT); //open the file for truncating and set the stream
                }
                else if (input.out == 2){// if out = 2 we append file 
                                file2=open(input.outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                }

                    
                
                    
                    //If exe's args are not null then add them to the list of args to pass
                if (input.exe_arg1.compare("") != 0){
                            char * temp = new char [input.exe_arg1.length()+1];
                            strcpy(temp, input.exe_arg1.c_str());
                            argv[0] = temp; 
                }
                if (input.exe_arg2.compare("") != 0){
                            char * temp = new char [input.exe_arg2.length()+1];
                            strcpy(temp, input.exe_arg2.c_str());
                            argv[1] = temp;
                }


                ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); //close stdin
                            close(0); //close stdout
                            dup2(file2,1); //redirect stdout to the out file 
                            dup2(file,0); //redirect stdin to the in file 
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  



            dup2(1,file2); //resets the input and output
            dup2(0,file); 
            close(file); 
            close(file2); 






 
        }//ends redirecting input and output 
        // if we are just using a pipe 
        else if (input.pipe ==1 && input.inp == 0  && input.out == 0 ){
            int fd[2]; 
            
            pid_t ChildPID;
            pipe(fd); 
            ChildPID= fork(); 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv_in[2]; 
            char * argv_out[2]; 
            string loc= cur_Path+"/"+input.pipe_in; //creates the path from the in pipe
            string loc2 = cur_Path+"/"+input.pipe_out; //creates the path for the out pipe 
            char * cstr_in = new char [loc.length()+1];
            char* cstr_out = new char [loc2.length()+1];
            strcpy(cstr_in, loc.c_str());
            strcpy(cstr_out, loc2.c_str());
            

                // obtain the args for pin 
                if (input.pin_arg1.compare("") != 0){
                                char * temp = new char [input.pin_arg1.length()+1];
                                strcpy(temp, input.pin_arg1.c_str());
                                argv_in[0] = temp; 
                    }
                if (input.pin_arg2.compare("") != 0){
                                char * temp = new char [input.pin_arg2.length()+1];
                                strcpy(temp, input.pin_arg2.c_str());
                                argv_in[1] = temp;
                }
                //obtain the args for Pout 
                if (input.pout_arg1.compare("") != 0){
                                char * temp = new char [input.pout_arg1.length()+1];
                                strcpy(temp, input.pout_arg1.c_str());
                                argv_out[0] = temp; 
                    }
                if (input.pout_arg2.compare("") != 0){
                                char * temp = new char [input.pout_arg2.length()+1];
                                strcpy(temp, input.pout_arg2.c_str());
                                argv_out[1] = temp;
                }





             
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); //close stdout
                           
                            dup2(fd[1],1); //redirect stdout to the in file 
                            close(fd[0]); //close read end of pipe 
                             
                            er= execv(cstr_in, argv_in);
                            
                            }
                            else { //parent process 
                               int child2 = fork(); 
                                if(child2 >=0){
                                    if (child2 == 0){
                                        close(0); //close stdin
                                        close(1); 
                                        dup2(fd[0],0); //redirect stdin to the infile 
                                        close (fd[1]); 
                                        execv(cstr_out, argv_out); 

                                    }
                                    else { // parent process 
                                        wait(0); 
                                    }
                                } //fork succeeded  
                                else { //fork failed 
                                    cout << "fork failed";
                                }
                                
                               
                                
                                
                                
                                
                                 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  



        //reset stdout and stdin
        dup2(1, fd[1]);
        dup2(0, fd[0]); 
        

        }

        //if we using a pipe and some kind of redirection 
        else if(input.pipe == 1 && ((input.out == 1 ||input.out == 2) || input.inp == 1)){
           int output = 0; 
           bool inp = false; 
           int file;
           int file2;
           int fd[2]; 
           
           
                if (input.out == 1){
                    output = 1; 
                    file2=open(input.outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT);
                }
                else if (input.out ==2){
                    output = 2; 
                    file2=open(input.outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                }
                if (input.inp == 1){
                    inp = true; 
                    file = open(input.infile.c_str(), O_RDONLY); 
                }


            pid_t ChildPID;
            pipe(fd); 
            ChildPID= fork(); 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv_in[2]; 
            char * argv_out[2]; 
            string loc= cur_Path+"/"+input.pipe_in; //creates the path from the in pipe
            string loc2 = cur_Path+"/"+input.pipe_out; //creates the path for the out pipe 
            char * cstr_in = new char [loc.length()+1];
            char* cstr_out = new char [loc2.length()+1];
            strcpy(cstr_in, loc.c_str());
            strcpy(cstr_out, loc2.c_str());
            

                // obtain the args for pin 
                if (input.pin_arg1.compare("") != 0){
                                char * temp = new char [input.pin_arg1.length()+1];
                                strcpy(temp, input.pin_arg1.c_str());
                                argv_in[0] = temp; 
                    }
                if (input.pin_arg2.compare("") != 0){
                                char * temp = new char [input.pin_arg2.length()+1];
                                strcpy(temp, input.pin_arg2.c_str());
                                argv_in[1] = temp;
                }
                //obtain the args for Pout 
                if (input.pout_arg1.compare("") != 0){
                                char * temp = new char [input.pout_arg1.length()+1];
                                strcpy(temp, input.pout_arg1.c_str());
                                argv_out[0] = temp; 
                    }
                if (input.pout_arg2.compare("") != 0){
                                char * temp = new char [input.pout_arg2.length()+1];
                                strcpy(temp, input.pout_arg2.c_str());
                                argv_out[1] = temp;
                }
            



            



        }//ends pipe & redirection 

        
        else {
            cout << user_input << " is Not a Valid Command\n";
            init_struct(input);
        }
    
    dup2(stdin_backup, 0);
    dup2(stdout_backup, 1); 
     
    }
    /*
        Runs the parallel commands 
        exact copy of the above loop with minor adjustments
    */
    else {
        for(int n=0;n<-para_cmd.size();n++){
            init_struct(para_cmd[n]);
        }
        
        para_cmd = get_para_command(user_input);
        //runs the parallel commands untill we reach the size of para_cmd
        for (int u =0; u<=para_cmd.size();u++){
            

        if(para_cmd[u].background){
            close(0);
            close(1);
        }


         // if cmd = quit 
        if (para_cmd[u].cmd.compare("quit") == 0){
            exit(0);  
        }
           // if cmd = clr 
        else if (para_cmd[u].cmd.compare("clr") == 0){
            clear_scrn();
        }
           // if cmd = cd 
        else if (para_cmd[u].cmd.compare("cd") == 0){
           if(chng_dir(para_cmd[u].cmd_arg) == 0) // if chng_dir returns true 
                cur_Path = getcwd(buff,buffsize); //set the current path to be displayed wit the current path
            else
            {
                cout << para_cmd[u].cmd_arg << " is not a valid path \n";            }
            
        }
               // if cmd = dir 
        else if (para_cmd[u].cmd.compare("dir") == 0&& para_cmd[u].out == 0) {
            ls(cur_Path);
        }
           // if cmd = environ
        else if(para_cmd[u].cmd.compare("environ")==0&& para_cmd[u].out == 0){
            print_enviroment(); 
        }
               // if cmd = echo 
        else if (para_cmd[u].cmd.compare("echo")==0&& para_cmd[u].out == 0){
            cout << para_cmd[u].cmd_arg << endl; 
        }
           // if cmd = pause 
        else if (para_cmd[u].cmd.compare("pause")==0){
            
            do {
                cout << "press enter to continue"; 
            }while(cin.get() != '\n');
        
        }
           // if cmd = help 
        else if (para_cmd[u].cmd.compare("help")==0 && para_cmd[u].out == 0){
            cur_Path = getcwd(buff,buffsize);
            string shell_path = ""; 
            shell_path = env_var; 
            if(chng_dir(env_var) == 0){
                ifstream File; 
                File.open("readme");
                    if (!File){
                        cout << "Can't open file" << endl; 
                        
                    }
                
                cout << File.rdbuf();
                chng_dir(cur_Path); 
                File.close();
            }   
            else {
                cout << "error in help\n"; 
            }
                
                
        } //if we are stricly executing an exe with no redirection 
        else if (para_cmd[u].exe.compare("") != 0 && !para_cmd[u].pipe && para_cmd[u].out == 0 && para_cmd[u].inp == 0){ // if exe is not null 
            
            
            pid_t ChildPID; 
            string enviroment = cur_Path; 
            ChildPID= fork(); 
            int er = 0; 
            char * argv[2]; 
            string file= cur_Path+"/"+para_cmd[u].exe;
            char * cstr = new char [file.length()+1];
            strcpy(cstr, file.c_str());
           
            
            //If exe's args are not null then add them to the list of args to pass
            if (para_cmd[u].exe_arg1.compare("") != 0){
                char * temp = new char [para_cmd[u].exe_arg1.length()+1];
                strcpy(temp, para_cmd[u].exe_arg1.c_str());
                argv[0] = temp; 
            }
            if (para_cmd[u].exe_arg2.compare("") != 0){
                 char * temp = new char [para_cmd[u].exe_arg2.length()+1];
                strcpy(temp, para_cmd[u].exe_arg2.c_str());
                argv[1] = temp;
            }


            //executing the program 
            if (ChildPID >= 0){  // fork was successful 
                if (ChildPID == 0){ //child process 
                  er= execv(cstr, argv);
                   
                
                }
                else { //parent process 
                    wait(0); 
                }
            }
            else{//fork failed
                perror("fork failed");
            }  
    
        }
        // if we are just redirection para_cmd[u] 
        else if (para_cmd[u].inp == 1 && para_cmd[u].out == 0){
            int file2; 
            file2 = open(para_cmd[u].infile.c_str(), O_RDONLY ); 
            int backup=dup(0); 
                     

                pid_t ChildPID; 
                string enviroment = cur_Path; 
                int er = 0; 
                char * argv[2]; 
                string file= cur_Path+"/"+para_cmd[u].exe;
                char * cstr = new char [file.length()+1];
                strcpy(cstr, file.c_str());
            
                    
                 //If exe's args are not null then add them to the list of args to pass
                    if (para_cmd[u].exe_arg1.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg1.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg1.c_str());
                            argv[0] = temp; 
                    }
                    if (para_cmd[u].exe_arg2.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg2.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg2.c_str());
                            argv[1] = temp;
                    }
                        
                        ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(0); 
                            dup2(file2,0);
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  
            
                        
                        
                        
                        
                 dup2(0,file2); //reset the stream 
                 close(file2); 


        }// end stricly file para_cmd[u] redirection 
        // if we are just redirecting output > means create and truncate
        else if ((para_cmd[u].out == 1 || para_cmd[u].out == 2) && para_cmd[u].inp == 0){
            int buffsize1 = 100000; 
            char buff1[buffsize1];
            fstream file;
            int file2;  
            streambuf* stream_buffer_cout = cout.rdbuf(); //backing up cout streambuffer
            int out_type = 0; // 1=exe , 2 = interal command 
                
                if (para_cmd[u].cmd.compare("")!=0){
                    out_type = 2;
                }
                else {
                    out_type = 1; 
                }

            
                
            
                if (out_type == 2){ //handle internal command first 
                        if (para_cmd[u].out == 1 ){ // if out = 1 we truncate file 
                                file.open(para_cmd[u].outfile, fstream::trunc | fstream::out); //open the file for truncating and set the stream
                        }
                        else if (para_cmd[u].out == 2){// if out = 2 we append file 
                                file.open(para_cmd[u].outfile, fstream::app | fstream::out);
                        }
                        
                    streambuf* strm_buff_file = file.rdbuf(); //get file stream buff
                    cout.rdbuf(strm_buff_file); //redriect cout to the file stream buff 
                        if (para_cmd[u].cmd.compare("help")==0){

                            cur_Path = getcwd(buff1,buffsize1);
                            string shell_path = ""; 
                            shell_path = env_var; 
                                if(chng_dir(env_var) == 0){
                                    ifstream Fi; 
                                    Fi.open("readme");
                                        if (!Fi){
                                            cout << "Can't open file" << endl; 
                                                
                                        }
                                        
                                    cout << Fi.rdbuf();
                                    chng_dir(cur_Path); 
                                    Fi.close();
                                    }   
                                    else {
                                        cout << "error in help\n"; 
                                    }
                        }///ends help 
                    else if(para_cmd[u].cmd.compare("dir")==0){
                        ls(cur_Path);
                    }
                    else if (para_cmd[u].cmd.compare("echo")==0){
                        cout << para_cmd[u].cmd_arg << endl; 
                    }
                    else if (para_cmd[u].cmd.compare("environ")==0){
                        print_enviroment(); 
                    }

                    cout.rdbuf(stream_buffer_cout); //resets the stream buffer back to cout 
                    file.close(); 
                 }
                else if (out_type == 1){ //handles executable writing to an out file 
                    
                    if (para_cmd[u].out == 1 ){ // if out = 1 we truncate file 
                                file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT); //open the file for truncating and set the stream
                    }
                    else if (para_cmd[u].out == 2){// if out = 2 we append file 
                                file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                    }

                    
                    int backup=dup(1); 
                     

                    pid_t ChildPID; 
                    string enviroment = cur_Path; 
                    int er = 0; 
                    char * argv[2]; 
                    string file= cur_Path+"/"+para_cmd[u].exe;
                    char * cstr = new char [file.length()+1];
                    strcpy(cstr, file.c_str());
                
                    
                    //If exe's args are not null then add them to the list of args to pass
                        if (para_cmd[u].exe_arg1.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg1.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg1.c_str());
                            argv[0] = temp; 
                        }
                        if (para_cmd[u].exe_arg2.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg2.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg2.c_str());
                            argv[1] = temp;
                        }

                        ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); 
                            dup2(file2,1);
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  
            
                        
                        
                        
                        
                 dup2(1,file2); //reset the stream  
                 
                close(file2);   
                
                }// ends redricting exe



                
                
        }
        //if we are redirecting both para_cmd[u] and output IE EXE is taking its para_cmd[u] from 
        // para_cmd[u].infile and its output is being redirected to para_cmd[u].outfile
        else if ((para_cmd[u].out == 1 || para_cmd[u].out == 2) && para_cmd[u].inp == 1){
            int file2;
            int file = open(para_cmd[u].infile.c_str(), O_RDONLY); 
            int in_back = dup(0);
            int out_back = dup(1); 
            pid_t ChildPID; 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv[2]; 
            string loc= cur_Path+"/"+para_cmd[u].exe;
            char * cstr = new char [loc.length()+1];
            strcpy(cstr, loc.c_str());

                   
                   
                   
                if (para_cmd[u].out == 1 ){ // if out = 1 we truncate file 
                                file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT); //open the file for truncating and set the stream
                }
                else if (para_cmd[u].out == 2){// if out = 2 we append file 
                                file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                }

                    
                
                    
                    //If exe's args are not null then add them to the list of args to pass
                if (para_cmd[u].exe_arg1.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg1.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg1.c_str());
                            argv[0] = temp; 
                }
                if (para_cmd[u].exe_arg2.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg2.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg2.c_str());
                            argv[1] = temp;
                }


                ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); //close stdin
                            close(0); //close stdout
                            dup2(file2,1); //redirect stdout to the out file 
                            dup2(file,0); //redirect stdin to the in file 
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  



            dup2(1,file2); //resets the para_cmd[u] and output
            dup2(0,file); 
            close(file); 
            close(file2); 






 
        }//ends redirecting para_cmd[u] and output 
        // if we are just using a pipe 
        else if (para_cmd[u].pipe ==1 && para_cmd[u].inp == 0  && para_cmd[u].out == 0 ){
            int fd[2]; 
            
            pid_t ChildPID;
            pipe(fd); 
            ChildPID= fork(); 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv_in[2]; 
            char * argv_out[2]; 
            string loc= cur_Path+"/"+para_cmd[u].pipe_in; //creates the path from the in pipe
            string loc2 = cur_Path+"/"+para_cmd[u].pipe_out; //creates the path for the out pipe 
            char * cstr_in = new char [loc.length()+1];
            char* cstr_out = new char [loc2.length()+1];
            strcpy(cstr_in, loc.c_str());
            strcpy(cstr_out, loc2.c_str());
            

                // obtain the args for pin 
                if (para_cmd[u].pin_arg1.compare("") != 0){
                                char * temp = new char [para_cmd[u].pin_arg1.length()+1];
                                strcpy(temp, para_cmd[u].pin_arg1.c_str());
                                argv_in[0] = temp; 
                    }
                if (para_cmd[u].pin_arg2.compare("") != 0){
                                char * temp = new char [para_cmd[u].pin_arg2.length()+1];
                                strcpy(temp, para_cmd[u].pin_arg2.c_str());
                                argv_in[1] = temp;
                }
                //obtain the args for Pout 
                if (para_cmd[u].pout_arg1.compare("") != 0){
                                char * temp = new char [para_cmd[u].pout_arg1.length()+1];
                                strcpy(temp, para_cmd[u].pout_arg1.c_str());
                                argv_out[0] = temp; 
                    }
                if (para_cmd[u].pout_arg2.compare("") != 0){
                                char * temp = new char [para_cmd[u].pout_arg2.length()+1];
                                strcpy(temp, para_cmd[u].pout_arg2.c_str());
                                argv_out[1] = temp;
                }





             
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); //close stdout
                           
                            dup2(fd[1],1); //redirect stdout to the in file 
                            close(fd[0]); //close read end of pipe 
                             
                            er= execv(cstr_in, argv_in);
                            
                            }
                            else { //parent process 
                               int child2 = fork(); 
                                if(child2 >=0){
                                    if (child2 == 0){
                                        close(0); //close stdin
                                        close(1); 
                                        dup2(fd[0],0); //redirect stdin to the infile 
                                        close (fd[1]); 
                                        execv(cstr_out, argv_out); 

                                    }
                                    else { // parent process 
                                        wait(0); 
                                    }
                                } //fork succeeded  
                                else { //fork failed 
                                    cout << "fork failed";
                                }
                                
                               
                                
                                
                                
                                
                                 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  



        //reset stdout and stdin
        dup2(1, fd[1]);
        dup2(0, fd[0]); 
        

        }

        //if we using a pipe and some kind of redirection 
        else if(para_cmd[u].pipe == 1 && ((para_cmd[u].out == 1 ||para_cmd[u].out == 2) || para_cmd[u].inp == 1)){
           int output = 0; 
           bool inp = false; 
           int file;
           int file2;
           int fd[2]; 
           
           
                if (para_cmd[u].out == 1){
                    output = 1; 
                    file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT);
                }
                else if (para_cmd[u].out ==2){
                    output = 2; 
                    file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                }
                if (para_cmd[u].inp == 1){
                    inp = true; 
                    file = open(para_cmd[u].infile.c_str(), O_RDONLY); 
                }


            pid_t ChildPID;
            pipe(fd); 
            ChildPID= fork(); 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv_in[2]; 
            char * argv_out[2]; 
            string loc= cur_Path+"/"+para_cmd[u].pipe_in; //creates the path from the in pipe
            string loc2 = cur_Path+"/"+para_cmd[u].pipe_out; //creates the path for the out pipe 
            char * cstr_in = new char [loc.length()+1];
            char* cstr_out = new char [loc2.length()+1];
            strcpy(cstr_in, loc.c_str());
            strcpy(cstr_out, loc2.c_str());
            

                // obtain the args for pin 
                if (para_cmd[u].pin_arg1.compare("") != 0){
                                char * temp = new char [para_cmd[u].pin_arg1.length()+1];
                                strcpy(temp, para_cmd[u].pin_arg1.c_str());
                                argv_in[0] = temp; 
                    }
                if (para_cmd[u].pin_arg2.compare("") != 0){
                                char * temp = new char [para_cmd[u].pin_arg2.length()+1];
                                strcpy(temp, para_cmd[u].pin_arg2.c_str());
                                argv_in[1] = temp;
                }
                //obtain the args for Pout 
                if (para_cmd[u].pout_arg1.compare("") != 0){
                                char * temp = new char [para_cmd[u].pout_arg1.length()+1];
                                strcpy(temp, para_cmd[u].pout_arg1.c_str());
                                argv_out[0] = temp; 
                    }
                if (para_cmd[u].pout_arg2.compare("") != 0){
                                char * temp = new char [para_cmd[u].pout_arg2.length()+1];
                                strcpy(temp, para_cmd[u].pout_arg2.c_str());
                                argv_out[1] = temp;
                }
            



            



        }//ends pipe & redirection 

        
        else {
            cout << user_input << " is Not a Valid Command\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            
        }
    
    dup2(stdin_backup, 0);
    dup2(stdout_backup, 1); 
    } 
}
}






int main (int argc, char *argv[]){

    if (argc == 2){ // if argc > 1 then atleast one argument is entered 
        ifstream Batch; 
        string s = "";
        Batch.open(argv[1]);
            if (!Batch){
                        cout << "Can't open file" << endl;
                        
                        
            }
            else{
                 
                    while(getline(Batch,s)){
                        run_command(s);
                    }
            }

    }
    else if (argc > 2){ //if you pass more then 1 arg we exit() 
        exit(1); 
    }




 int buffsize = 100000; 
    char buff[buffsize];
    int stdout_backup = dup(1);
int stdin_backup = dup(0);
    string cur_Path = ""; 
    command input;
    vector <command> para_cmd;
    string user_input = "";
    string pattern = " & ";


env_var = getcwd(buff,buffsize); 




int g = 0;

while (g == 0){
    
    user_input = "";
    init_struct(input); /// sets input to null each time the loop runs
    cur_Path = getcwd(buff,buffsize);
    cout << "MyShell>" + cur_Path + "$ "; //prints the name of the shell + the current path
    
    getline(cin, user_input); 

    /*
    Runs the shell if no parallel commands were used 
    */



    if (countOccurence(pattern, user_input)==0){
        

        input = get_command(user_input); 
        if(input.background){
            close(0);
            close(1);
        }


         // if cmd = quit 
        if (input.cmd.compare("quit") == 0){
            exit(0);  
        }
           // if cmd = clr 
        else if (input.cmd.compare("clr") == 0){
            clear_scrn();
        }
           // if cmd = cd 
        else if (input.cmd.compare("cd") == 0){
           if(chng_dir(input.cmd_arg) == 0) // if chng_dir returns true 
                cur_Path = getcwd(buff,buffsize); //set the current path to be displayed wit the current path
            else
            {
                cout << input.cmd_arg << " is not a valid path \n";            }
            
        }
               // if cmd = dir 
        else if (input.cmd.compare("dir") == 0&& input.out == 0) {
            ls(cur_Path);
        }
           // if cmd = environ
        else if(input.cmd.compare("environ")==0&& input.out == 0){
            print_enviroment(); 
        }
               // if cmd = echo 
        else if (input.cmd.compare("echo")==0&& input.out == 0){
            cout << input.cmd_arg << endl; 
        }
           // if cmd = pause 
        else if (input.cmd.compare("pause")==0){
            
            do {
                cout << "press enter to continue"; 
            }while(cin.get() != '\n');
        
        }
           // if cmd = help 
        else if (input.cmd.compare("help")==0 && input.out == 0){
            cur_Path = getcwd(buff,buffsize);
            string shell_path = ""; 
            shell_path = env_var; 
            if(chng_dir(env_var) == 0){
                ifstream File; 
                File.open("readme");
                    if (!File){
                        cout << "Can't open file" << endl; 
                        
                    }
                
                cout << File.rdbuf();
                chng_dir(cur_Path); 
                File.close();
            }   
            else {
                cout << "error in help\n"; 
            }
                
                
        } //if we are stricly executing an exe with no redirection 
        else if (input.exe.compare("") != 0 && !input.pipe && input.out == 0 && input.inp == 0){ // if exe is not null 
            
            
            pid_t ChildPID; 
            string enviroment = cur_Path; 
            ChildPID= fork(); 
            int er = 0; 
            char * argv[2]; 
            string file= cur_Path+"/"+input.exe;
            char * cstr = new char [file.length()+1];
            strcpy(cstr, file.c_str());
           
            
            //If exe's args are not null then add them to the list of args to pass
            if (input.exe_arg1.compare("") != 0){
                char * temp = new char [input.exe_arg1.length()+1];
                strcpy(temp, input.exe_arg1.c_str());
                argv[0] = temp; 
            }
            if (input.exe_arg2.compare("") != 0){
                 char * temp = new char [input.exe_arg2.length()+1];
                strcpy(temp, input.exe_arg2.c_str());
                argv[1] = temp;
            }


            //executing the program 
            if (ChildPID >= 0){  // fork was successful 
                if (ChildPID == 0){ //child process 
                  er= execv(cstr, argv);
                   
                
                }
                else { //parent process 
                    wait(0); 
                }
            }
            else{//fork failed
                perror("fork failed");
            }  
    
        }
        // if we are just redirection input 
        else if (input.inp == 1 && input.out == 0){
            int file2; 
            file2 = open(input.infile.c_str(), O_RDONLY ); 
            int backup=dup(0); 
                     

                pid_t ChildPID; 
                string enviroment = cur_Path; 
                int er = 0; 
                char * argv[2]; 
                string file= cur_Path+"/"+input.exe;
                char * cstr = new char [file.length()+1];
                strcpy(cstr, file.c_str());
            
                    
                 //If exe's args are not null then add them to the list of args to pass
                    if (input.exe_arg1.compare("") != 0){
                            char * temp = new char [input.exe_arg1.length()+1];
                            strcpy(temp, input.exe_arg1.c_str());
                            argv[0] = temp; 
                    }
                    if (input.exe_arg2.compare("") != 0){
                            char * temp = new char [input.exe_arg2.length()+1];
                            strcpy(temp, input.exe_arg2.c_str());
                            argv[1] = temp;
                    }
                        
                        ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(0); 
                            dup2(file2,0);
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  
            
                        
                        
                        
                        
                 dup2(0,file2); //reset the stream 
                 close(file2); 


        }// end stricly file input redirection 
        // if we are just redirecting output > means create and truncate
        else if ((input.out == 1 || input.out == 2) && input.inp == 0){
            int buffsize1 = 100000; 
            char buff1[buffsize1];
            fstream file;
            int file2;  
            streambuf* stream_buffer_cout = cout.rdbuf(); //backing up cout streambuffer
            int out_type = 0; // 1=exe , 2 = interal command 
                
                if (input.cmd.compare("")!=0){
                    out_type = 2;
                }
                else {
                    out_type = 1; 
                }

            
                
            
                if (out_type == 2){ //handle internal command first 
                        if (input.out == 1 ){ // if out = 1 we truncate file 
                                file.open(input.outfile, fstream::trunc | fstream::out); //open the file for truncating and set the stream
                        }
                        else if (input.out == 2){// if out = 2 we append file 
                                file.open(input.outfile, fstream::app | fstream::out);
                        }
                        
                    streambuf* strm_buff_file = file.rdbuf(); //get file stream buff
                    cout.rdbuf(strm_buff_file); //redriect cout to the file stream buff 
                        if (input.cmd.compare("help")==0){

                            cur_Path = getcwd(buff1,buffsize1);
                            string shell_path = ""; 
                            shell_path = env_var; 
                                if(chng_dir(env_var) == 0){
                                    ifstream Fi; 
                                    Fi.open("readme");
                                        if (!Fi){
                                            cout << "Can't open file" << endl; 
                                                
                                        }
                                        
                                    cout << Fi.rdbuf();
                                    chng_dir(cur_Path); 
                                    Fi.close();
                                    }   
                                    else {
                                        cout << "error in help\n"; 
                                    }
                        }///ends help 
                    else if(input.cmd.compare("dir")==0){
                        ls(cur_Path);
                    }
                    else if (input.cmd.compare("echo")==0){
                        cout << input.cmd_arg << endl; 
                    }
                    else if (input.cmd.compare("environ")==0){
                        print_enviroment(); 
                    }

                    cout.rdbuf(stream_buffer_cout); //resets the stream buffer back to cout 
                    file.close(); 
                 }
                else if (out_type == 1){ //handles executable writing to an out file 
                    
                    if (input.out == 1 ){ // if out = 1 we truncate file 
                                file2=open(input.outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT); //open the file for truncating and set the stream
                    }
                    else if (input.out == 2){// if out = 2 we append file 
                                file2=open(input.outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                    }

                    
                    int backup=dup(1); 
                     

                    pid_t ChildPID; 
                    string enviroment = cur_Path; 
                    int er = 0; 
                    char * argv[2]; 
                    string file= cur_Path+"/"+input.exe;
                    char * cstr = new char [file.length()+1];
                    strcpy(cstr, file.c_str());
                
                    
                    //If exe's args are not null then add them to the list of args to pass
                        if (input.exe_arg1.compare("") != 0){
                            char * temp = new char [input.exe_arg1.length()+1];
                            strcpy(temp, input.exe_arg1.c_str());
                            argv[0] = temp; 
                        }
                        if (input.exe_arg2.compare("") != 0){
                            char * temp = new char [input.exe_arg2.length()+1];
                            strcpy(temp, input.exe_arg2.c_str());
                            argv[1] = temp;
                        }

                        ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); 
                            dup2(file2,1);
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  
            
                        
                        
                        
                        
                 dup2(1,file2); //reset the stream  
                 
                close(file2);   
                
                }// ends redricting exe



                
                
        }
        //if we are redirecting both input and output IE EXE is taking its input from 
        // input.infile and its output is being redirected to input.outfile
        else if ((input.out == 1 || input.out == 2) && input.inp == 1){
            int file2;
            int file = open(input.infile.c_str(), O_RDONLY); 
            int in_back = dup(0);
            int out_back = dup(1); 
            pid_t ChildPID; 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv[2]; 
            string loc= cur_Path+"/"+input.exe;
            char * cstr = new char [loc.length()+1];
            strcpy(cstr, loc.c_str());

                   
                   
                   
                if (input.out == 1 ){ // if out = 1 we truncate file 
                                file2=open(input.outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT); //open the file for truncating and set the stream
                }
                else if (input.out == 2){// if out = 2 we append file 
                                file2=open(input.outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                }

                    
                
                    
                    //If exe's args are not null then add them to the list of args to pass
                if (input.exe_arg1.compare("") != 0){
                            char * temp = new char [input.exe_arg1.length()+1];
                            strcpy(temp, input.exe_arg1.c_str());
                            argv[0] = temp; 
                }
                if (input.exe_arg2.compare("") != 0){
                            char * temp = new char [input.exe_arg2.length()+1];
                            strcpy(temp, input.exe_arg2.c_str());
                            argv[1] = temp;
                }


                ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); //close stdin
                            close(0); //close stdout
                            dup2(file2,1); //redirect stdout to the out file 
                            dup2(file,0); //redirect stdin to the in file 
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  



            dup2(1,file2); //resets the input and output
            dup2(0,file); 
            close(file); 
            close(file2); 






 
        }//ends redirecting input and output 
        // if we are just using a pipe 
        else if (input.pipe ==1 && input.inp == 0  && input.out == 0 ){
            int fd[2]; 
            
            pid_t ChildPID;
            pipe(fd); 
            ChildPID= fork(); 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv_in[2]; 
            char * argv_out[2]; 
            string loc= cur_Path+"/"+input.pipe_in; //creates the path from the in pipe
            string loc2 = cur_Path+"/"+input.pipe_out; //creates the path for the out pipe 
            char * cstr_in = new char [loc.length()+1];
            char* cstr_out = new char [loc2.length()+1];
            strcpy(cstr_in, loc.c_str());
            strcpy(cstr_out, loc2.c_str());
            

                // obtain the args for pin 
                if (input.pin_arg1.compare("") != 0){
                                char * temp = new char [input.pin_arg1.length()+1];
                                strcpy(temp, input.pin_arg1.c_str());
                                argv_in[0] = temp; 
                    }
                if (input.pin_arg2.compare("") != 0){
                                char * temp = new char [input.pin_arg2.length()+1];
                                strcpy(temp, input.pin_arg2.c_str());
                                argv_in[1] = temp;
                }
                //obtain the args for Pout 
                if (input.pout_arg1.compare("") != 0){
                                char * temp = new char [input.pout_arg1.length()+1];
                                strcpy(temp, input.pout_arg1.c_str());
                                argv_out[0] = temp; 
                    }
                if (input.pout_arg2.compare("") != 0){
                                char * temp = new char [input.pout_arg2.length()+1];
                                strcpy(temp, input.pout_arg2.c_str());
                                argv_out[1] = temp;
                }





             
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); //close stdout
                           
                            dup2(fd[1],1); //redirect stdout to the in file 
                            close(fd[0]); //close read end of pipe 
                             
                            er= execv(cstr_in, argv_in);
                            
                            }
                            else { //parent process 
                               int child2 = fork(); 
                                if(child2 >=0){
                                    if (child2 == 0){
                                        close(0); //close stdin
                                        close(1); 
                                        dup2(fd[0],0); //redirect stdin to the infile 
                                        close (fd[1]); 
                                        execv(cstr_out, argv_out); 

                                    }
                                    else { // parent process 
                                        wait(0); 
                                    }
                                } //fork succeeded  
                                else { //fork failed 
                                    cout << "fork failed";
                                }
                                
                               
                                
                                
                                
                                
                                 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  



        //reset stdout and stdin
        dup2(1, fd[1]);
        dup2(0, fd[0]); 
        

        }

        //if we using a pipe and some kind of redirection 
        else if(input.pipe == 1 && ((input.out == 1 ||input.out == 2) || input.inp == 1)){
           int output = 0; 
           bool inp = false; 
           int file;
           int file2;
           int fd[2]; 
           
           
                if (input.out == 1){
                    output = 1; 
                    file2=open(input.outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT);
                }
                else if (input.out ==2){
                    output = 2; 
                    file2=open(input.outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                }
                if (input.inp == 1){
                    inp = true; 
                    file = open(input.infile.c_str(), O_RDONLY); 
                }


            pid_t ChildPID;
            pipe(fd); 
            ChildPID= fork(); 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv_in[2]; 
            char * argv_out[2]; 
            string loc= cur_Path+"/"+input.pipe_in; //creates the path from the in pipe
            string loc2 = cur_Path+"/"+input.pipe_out; //creates the path for the out pipe 
            char * cstr_in = new char [loc.length()+1];
            char* cstr_out = new char [loc2.length()+1];
            strcpy(cstr_in, loc.c_str());
            strcpy(cstr_out, loc2.c_str());
            

                // obtain the args for pin 
                if (input.pin_arg1.compare("") != 0){
                                char * temp = new char [input.pin_arg1.length()+1];
                                strcpy(temp, input.pin_arg1.c_str());
                                argv_in[0] = temp; 
                    }
                if (input.pin_arg2.compare("") != 0){
                                char * temp = new char [input.pin_arg2.length()+1];
                                strcpy(temp, input.pin_arg2.c_str());
                                argv_in[1] = temp;
                }
                //obtain the args for Pout 
                if (input.pout_arg1.compare("") != 0){
                                char * temp = new char [input.pout_arg1.length()+1];
                                strcpy(temp, input.pout_arg1.c_str());
                                argv_out[0] = temp; 
                    }
                if (input.pout_arg2.compare("") != 0){
                                char * temp = new char [input.pout_arg2.length()+1];
                                strcpy(temp, input.pout_arg2.c_str());
                                argv_out[1] = temp;
                }
            



            



        }//ends pipe & redirection 

        
        else {
            cout << user_input << " is Not a Valid Command\n";
            init_struct(input);
        }
    
    dup2(stdin_backup, 0);
    dup2(stdout_backup, 1); 
     
    }
    /*
        Runs the parallel commands 
        exact copy of the above loop with minor adjustments
    */
    else {
        for(int n=0;n<-para_cmd.size();n++){
            init_struct(para_cmd[n]);
        }
        
        para_cmd = get_para_command(user_input);
        //runs the parallel commands untill we reach the size of para_cmd
        for (int u =0; u<=para_cmd.size()-1;u++){
            

        if(para_cmd[u].background){
            close(0);
            close(1);
        }


         // if cmd = quit 
        if (para_cmd[u].cmd.compare("quit") == 0){
            exit(0);  
        }
           // if cmd = clr 
        else if (para_cmd[u].cmd.compare("clr") == 0){
            clear_scrn();
        }
           // if cmd = cd 
        else if (para_cmd[u].cmd.compare("cd") == 0){
           if(chng_dir(para_cmd[u].cmd_arg) == 0) // if chng_dir returns true 
                cur_Path = getcwd(buff,buffsize); //set the current path to be displayed wit the current path
            else
            {
                cout << para_cmd[u].cmd_arg << " is not a valid path \n";            }
            
        }
               // if cmd = dir 
        else if (para_cmd[u].cmd.compare("dir") == 0&& para_cmd[u].out == 0) {
            ls(cur_Path);
        }
           // if cmd = environ
        else if(para_cmd[u].cmd.compare("environ")==0&& para_cmd[u].out == 0){
            print_enviroment(); 
        }
               // if cmd = echo 
        else if (para_cmd[u].cmd.compare("echo")==0&& para_cmd[u].out == 0){
            cout << para_cmd[u].cmd_arg << endl; 
        }
           // if cmd = pause 
        else if (para_cmd[u].cmd.compare("pause")==0){
            
            do {
                cout << "press enter to continue"; 
            }while(cin.get() != '\n');
        
        }
           // if cmd = help 
        else if (para_cmd[u].cmd.compare("help")==0 && para_cmd[u].out == 0){
            cur_Path = getcwd(buff,buffsize);
            string shell_path = ""; 
            shell_path = env_var; 
            if(chng_dir(env_var) == 0){
                ifstream File; 
                File.open("readme");
                    if (!File){
                        cout << "Can't open file" << endl; 
                        
                    }
                
                cout << File.rdbuf();
                chng_dir(cur_Path); 
                File.close();
            }   
            else {
                cout << "error in help\n"; 
            }
                
                
        } //if we are stricly executing an exe with no redirection 
        else if (para_cmd[u].exe.compare("") != 0 && !para_cmd[u].pipe && para_cmd[u].out == 0 && para_cmd[u].inp == 0){ // if exe is not null 
            
            
            pid_t ChildPID; 
            string enviroment = cur_Path; 
            ChildPID= fork(); 
            int er = 0; 
            char * argv[2]; 
            string file= cur_Path+"/"+para_cmd[u].exe;
            char * cstr = new char [file.length()+1];
            strcpy(cstr, file.c_str());
           
            
            //If exe's args are not null then add them to the list of args to pass
            if (para_cmd[u].exe_arg1.compare("") != 0){
                char * temp = new char [para_cmd[u].exe_arg1.length()+1];
                strcpy(temp, para_cmd[u].exe_arg1.c_str());
                argv[0] = temp; 
            }
            if (para_cmd[u].exe_arg2.compare("") != 0){
                 char * temp = new char [para_cmd[u].exe_arg2.length()+1];
                strcpy(temp, para_cmd[u].exe_arg2.c_str());
                argv[1] = temp;
            }


            //executing the program 
            if (ChildPID >= 0){  // fork was successful 
                if (ChildPID == 0){ //child process 
                  er= execv(cstr, argv);
                   
                
                }
                else { //parent process 
                    wait(0); 
                }
            }
            else{//fork failed
                perror("fork failed");
            }  
    
        }
        // if we are just redirection para_cmd[u] 
        else if (para_cmd[u].inp == 1 && para_cmd[u].out == 0){
            int file2; 
            file2 = open(para_cmd[u].infile.c_str(), O_RDONLY ); 
            int backup=dup(0); 
                     

                pid_t ChildPID; 
                string enviroment = cur_Path; 
                int er = 0; 
                char * argv[2]; 
                string file= cur_Path+"/"+para_cmd[u].exe;
                char * cstr = new char [file.length()+1];
                strcpy(cstr, file.c_str());
            
                    
                 //If exe's args are not null then add them to the list of args to pass
                    if (para_cmd[u].exe_arg1.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg1.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg1.c_str());
                            argv[0] = temp; 
                    }
                    if (para_cmd[u].exe_arg2.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg2.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg2.c_str());
                            argv[1] = temp;
                    }
                        
                        ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(0); 
                            dup2(file2,0);
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  
            
                        
                        
                        
                        
                 dup2(0,file2); //reset the stream 
                 close(file2); 


        }// end stricly file para_cmd[u] redirection 
        // if we are just redirecting output > means create and truncate
        else if ((para_cmd[u].out == 1 || para_cmd[u].out == 2) && para_cmd[u].inp == 0){
            int buffsize1 = 100000; 
            char buff1[buffsize1];
            fstream file;
            int file2;  
            streambuf* stream_buffer_cout = cout.rdbuf(); //backing up cout streambuffer
            int out_type = 0; // 1=exe , 2 = interal command 
                
                if (para_cmd[u].cmd.compare("")!=0){
                    out_type = 2;
                }
                else {
                    out_type = 1; 
                }

            
                
            
                if (out_type == 2){ //handle internal command first 
                        if (para_cmd[u].out == 1 ){ // if out = 1 we truncate file 
                                file.open(para_cmd[u].outfile, fstream::trunc | fstream::out); //open the file for truncating and set the stream
                        }
                        else if (para_cmd[u].out == 2){// if out = 2 we append file 
                                file.open(para_cmd[u].outfile, fstream::app | fstream::out);
                        }
                        
                    streambuf* strm_buff_file = file.rdbuf(); //get file stream buff
                    cout.rdbuf(strm_buff_file); //redriect cout to the file stream buff 
                        if (para_cmd[u].cmd.compare("help")==0){

                            cur_Path = getcwd(buff1,buffsize1);
                            string shell_path = ""; 
                            shell_path = env_var; 
                                if(chng_dir(env_var) == 0){
                                    ifstream Fi; 
                                    Fi.open("readme");
                                        if (!Fi){
                                            cout << "Can't open file" << endl; 
                                                
                                        }
                                        
                                    cout << Fi.rdbuf();
                                    chng_dir(cur_Path); 
                                    Fi.close();
                                    }   
                                    else {
                                        cout << "error in help\n"; 
                                    }
                        }///ends help 
                    else if(para_cmd[u].cmd.compare("dir")==0){
                        ls(cur_Path);
                    }
                    else if (para_cmd[u].cmd.compare("echo")==0){
                        cout << para_cmd[u].cmd_arg << endl; 
                    }
                    else if (para_cmd[u].cmd.compare("environ")==0){
                        print_enviroment(); 
                    }

                    cout.rdbuf(stream_buffer_cout); //resets the stream buffer back to cout 
                    file.close(); 
                 }
                else if (out_type == 1){ //handles executable writing to an out file 
                    
                    if (para_cmd[u].out == 1 ){ // if out = 1 we truncate file 
                                file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT); //open the file for truncating and set the stream
                    }
                    else if (para_cmd[u].out == 2){// if out = 2 we append file 
                                file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                    }

                    
                    int backup=dup(1); 
                     

                    pid_t ChildPID; 
                    string enviroment = cur_Path; 
                    int er = 0; 
                    char * argv[2]; 
                    string file= cur_Path+"/"+para_cmd[u].exe;
                    char * cstr = new char [file.length()+1];
                    strcpy(cstr, file.c_str());
                
                    
                    //If exe's args are not null then add them to the list of args to pass
                        if (para_cmd[u].exe_arg1.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg1.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg1.c_str());
                            argv[0] = temp; 
                        }
                        if (para_cmd[u].exe_arg2.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg2.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg2.c_str());
                            argv[1] = temp;
                        }

                        ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); 
                            dup2(file2,1);
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  
            
                        
                        
                        
                        
                 dup2(1,file2); //reset the stream  
                 
                close(file2);   
                
                }// ends redricting exe



                
                
        }
        //if we are redirecting both para_cmd[u] and output IE EXE is taking its para_cmd[u] from 
        // para_cmd[u].infile and its output is being redirected to para_cmd[u].outfile
        else if ((para_cmd[u].out == 1 || para_cmd[u].out == 2) && para_cmd[u].inp == 1){
            int file2;
            int file = open(para_cmd[u].infile.c_str(), O_RDONLY); 
            int in_back = dup(0);
            int out_back = dup(1); 
            pid_t ChildPID; 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv[2]; 
            string loc= cur_Path+"/"+para_cmd[u].exe;
            char * cstr = new char [loc.length()+1];
            strcpy(cstr, loc.c_str());

                   
                   
                   
                if (para_cmd[u].out == 1 ){ // if out = 1 we truncate file 
                                file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT); //open the file for truncating and set the stream
                }
                else if (para_cmd[u].out == 2){// if out = 2 we append file 
                                file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                }

                    
                
                    
                    //If exe's args are not null then add them to the list of args to pass
                if (para_cmd[u].exe_arg1.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg1.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg1.c_str());
                            argv[0] = temp; 
                }
                if (para_cmd[u].exe_arg2.compare("") != 0){
                            char * temp = new char [para_cmd[u].exe_arg2.length()+1];
                            strcpy(temp, para_cmd[u].exe_arg2.c_str());
                            argv[1] = temp;
                }


                ChildPID= fork(); 
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); //close stdin
                            close(0); //close stdout
                            dup2(file2,1); //redirect stdout to the out file 
                            dup2(file,0); //redirect stdin to the in file 
                             
                            er= execv(cstr, argv);
                            
                            }
                            else { //parent process 
                                wait(0); 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  



            dup2(1,file2); //resets the para_cmd[u] and output
            dup2(0,file); 
            close(file); 
            close(file2); 






 
        }//ends redirecting para_cmd[u] and output 
        // if we are just using a pipe 
        else if (para_cmd[u].pipe ==1 && para_cmd[u].inp == 0  && para_cmd[u].out == 0 ){
            int fd[2]; 
            
            pid_t ChildPID;
            pipe(fd); 
            ChildPID= fork(); 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv_in[2]; 
            char * argv_out[2]; 
            string loc= cur_Path+"/"+para_cmd[u].pipe_in; //creates the path from the in pipe
            string loc2 = cur_Path+"/"+para_cmd[u].pipe_out; //creates the path for the out pipe 
            char * cstr_in = new char [loc.length()+1];
            char* cstr_out = new char [loc2.length()+1];
            strcpy(cstr_in, loc.c_str());
            strcpy(cstr_out, loc2.c_str());
            

                // obtain the args for pin 
                if (para_cmd[u].pin_arg1.compare("") != 0){
                                char * temp = new char [para_cmd[u].pin_arg1.length()+1];
                                strcpy(temp, para_cmd[u].pin_arg1.c_str());
                                argv_in[0] = temp; 
                    }
                if (para_cmd[u].pin_arg2.compare("") != 0){
                                char * temp = new char [para_cmd[u].pin_arg2.length()+1];
                                strcpy(temp, para_cmd[u].pin_arg2.c_str());
                                argv_in[1] = temp;
                }
                //obtain the args for Pout 
                if (para_cmd[u].pout_arg1.compare("") != 0){
                                char * temp = new char [para_cmd[u].pout_arg1.length()+1];
                                strcpy(temp, para_cmd[u].pout_arg1.c_str());
                                argv_out[0] = temp; 
                    }
                if (para_cmd[u].pout_arg2.compare("") != 0){
                                char * temp = new char [para_cmd[u].pout_arg2.length()+1];
                                strcpy(temp, para_cmd[u].pout_arg2.c_str());
                                argv_out[1] = temp;
                }





             
                         //executing the program 
                        if (ChildPID >= 0){  // fork was successful 
                            if (ChildPID == 0){ //child process 
                            close(1); //close stdout
                           
                            dup2(fd[1],1); //redirect stdout to the in file 
                            close(fd[0]); //close read end of pipe 
                             
                            er= execv(cstr_in, argv_in);
                            
                            }
                            else { //parent process 
                               int child2 = fork(); 
                                if(child2 >=0){
                                    if (child2 == 0){
                                        close(0); //close stdin
                                        close(1); 
                                        dup2(fd[0],0); //redirect stdin to the infile 
                                        close (fd[1]); 
                                        execv(cstr_out, argv_out); 

                                    }
                                    else { // parent process 
                                        wait(0); 
                                    }
                                } //fork succeeded  
                                else { //fork failed 
                                    cout << "fork failed";
                                }
                                
                               
                                
                                
                                
                                
                                 
                            }
                        }
                        else{//fork failed
                            perror("fork failed");
                        }  



        //reset stdout and stdin
        dup2(1, fd[1]);
        dup2(0, fd[0]); 
        

        }

        //if we using a pipe and some kind of redirection 
        else if(para_cmd[u].pipe == 1 && ((para_cmd[u].out == 1 ||para_cmd[u].out == 2) || para_cmd[u].inp == 1)){
           int output = 0; 
           bool inp = false; 
           int file;
           int file2;
           int fd[2]; 
           
           
                if (para_cmd[u].out == 1){
                    output = 1; 
                    file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_TRUNC | O_CREAT);
                }
                else if (para_cmd[u].out ==2){
                    output = 2; 
                    file2=open(para_cmd[u].outfile.c_str(),  O_RDWR | O_APPEND | O_CREAT);
                }
                if (para_cmd[u].inp == 1){
                    inp = true; 
                    file = open(para_cmd[u].infile.c_str(), O_RDONLY); 
                }


            pid_t ChildPID;
            pipe(fd); 
            ChildPID= fork(); 
            string enviroment = cur_Path; 
            int er = 0; 
            char * argv_in[2]; 
            char * argv_out[2]; 
            string loc= cur_Path+"/"+para_cmd[u].pipe_in; //creates the path from the in pipe
            string loc2 = cur_Path+"/"+para_cmd[u].pipe_out; //creates the path for the out pipe 
            char * cstr_in = new char [loc.length()+1];
            char* cstr_out = new char [loc2.length()+1];
            strcpy(cstr_in, loc.c_str());
            strcpy(cstr_out, loc2.c_str());
            

                // obtain the args for pin 
                if (para_cmd[u].pin_arg1.compare("") != 0){
                                char * temp = new char [para_cmd[u].pin_arg1.length()+1];
                                strcpy(temp, para_cmd[u].pin_arg1.c_str());
                                argv_in[0] = temp; 
                    }
                if (para_cmd[u].pin_arg2.compare("") != 0){
                                char * temp = new char [para_cmd[u].pin_arg2.length()+1];
                                strcpy(temp, para_cmd[u].pin_arg2.c_str());
                                argv_in[1] = temp;
                }
                //obtain the args for Pout 
                if (para_cmd[u].pout_arg1.compare("") != 0){
                                char * temp = new char [para_cmd[u].pout_arg1.length()+1];
                                strcpy(temp, para_cmd[u].pout_arg1.c_str());
                                argv_out[0] = temp; 
                    }
                if (para_cmd[u].pout_arg2.compare("") != 0){
                                char * temp = new char [para_cmd[u].pout_arg2.length()+1];
                                strcpy(temp, para_cmd[u].pout_arg2.c_str());
                                argv_out[1] = temp;
                }
            



            



        }//ends pipe & redirection 

        
        else {
            cout << user_input << " is Not a Valid Command\n";
            
        }
    
    dup2(stdin_backup, 0);
    dup2(stdout_backup, 1); 
    } 
    }









    }

    
    
    


       

        
  

   





return 0;     
}

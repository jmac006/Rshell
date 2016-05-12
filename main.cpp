#include <iostream>
#include <stdio.h>
#include <unistd.h> //used for fork(), execvp()
#include <sys/types.h> //used for wait
#include <sys/wait.h> //used for wait
#include <string.h>
#include <cstdlib>
#include <vector>

using namespace std;

bool execute(vector<string>commandArr) {
	int works = 1;
	/*if(line == "exit") {
		exit(0);
	}*/
	char* cmdArr[commandArr.size() + 1];
	int i = 0;
	for (i = 0; i < static_cast<int>(commandArr.size()); i++) {//convert the vector into a char* array for execvp
		cmdArr[i] = (char*)commandArr.at(i).c_str();	
	}
	cmdArr[commandArr.size()] = NULL; //set last value to NULL for execvp
	int status = 0;		
	char fullpath[20] = "/bin/"; //full file path, starts at bin	
	strcat(fullpath, commandArr[0].c_str()); //add the first command to the file path
	pid_t pid = fork();//split the processes into parent and child
	if(pid==0) { //if the process is a child
		int exec = execvp(fullpath,cmdArr); // to check if execvp passes or fails	
		if (exec == -1) { //execvp failed 
			perror("exec");
			exit(-1);
		}
	}
	else { //Otherwise, the process is a parent
		waitpid(pid, &status, 0); //wait for the child process to finish
	}
	if (works == -1) {
		return false;
	}
	else {
		return true;	
	}	

}

void parse_string(string commandLine, vector<string>&cmdArray) {
	
	char* token; //split command into separate strings
	char* cmd = new char[commandLine.length() + 1];
	strcpy(cmd, commandLine.c_str()); //converts string to char*
	token = strtok(cmd, " "); //tokenize first part of string
	int i = 0;
	for (; token != NULL; i++) {
		if (token == NULL) { //break out of for loop if there's an empty token
			break;
		}
		string tokenString = string(token);
		//cout << "Token is: " << tokenString << endl;
		cmdArray.push_back(tokenString);
		token = strtok(NULL, " ");
	}
	
} 

void printPrompt() {
	char* user = getlogin();
	char host[BUFSIZ]; //creates a host name with buffer size
	int hostID = gethostname(host, 1024);
	if (user == NULL) { //could not find the username
		cout << "$ ";
	}
	else {
		cout << user << '@' << host << "$ ";
	}
}

bool isConnector(string s) {
	if (s == "||" || s== "&&" || s == ";" || s == "#") {
		return true;
	}
	return false;	
}

int main () {
		string cmdLine;
		bool hasExecuted = true;	
		while(cmdLine != "exit") {	
			vector<string>cmdArr; //holds the whole command line (parsed)
			vector<string>command; //holds individual commands to run in execute() function
			printPrompt();
			getline(cin, cmdLine);
			if (cmdLine == "exit") { //exits with command "exit"
				exit(0);
			}

			parse_string(cmdLine, cmdArr);
			int index = 0;
			for (int i = 0; i < cmdArr.size(); i++) {
				index = i;
				if (!isConnector(cmdArr.at(i))) {
					command.push_back(cmdArr.at(i)); //copy the command from cmdArr to command vector if the command is not a connector
				}
				else { //if it finds a connector, break the loop so we only have the first command in our vector
					break;
				}
			}
			hasExecuted = execute(command); //execute the first command
			command.clear(); //clear the first command to read the other commands
			for (int i = index; i < cmdArr.size(); i++) { //start where we left off	
				command.push_back(cmdArr.at(i));
				
				if(isConnector(cmdArr.at(i)) && command.size() > 1) { //if there's more commands and the next command is a connector
					if(command.at(0) == "||") {
						command.erase(command.begin()); //delete the connector
						if (hasExecuted == false) { //if the previous command did not execute, run this command
							hasExecuted = execute(command);
						}
						command.clear();
					}
					else if (command.at(0) == "&&") {
						
						command.erase(command.begin()); //remove connector from command
						if (hasExecuted == true) {
							hasExecuted = execute(command);
						}
						command.clear();
					}
				}
			}
			if (command.size() != 0 && command.at(0) == "&&") {
				command.erase(command.begin()); //delete connector
				if (hasExecuted == true) {
					hasExecuted = execute(command);
				}
			}
			
		}
		return 0;
}

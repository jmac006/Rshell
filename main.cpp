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
	if(commandArr.size() > 1){
		if (commandArr.at(1).at(0) == '\"'||commandArr.at(1).at(0) ==  '\'') {
			commandArr.at(1).erase(commandArr.at(1).begin());
			commandArr.at(commandArr.size()-1).erase(commandArr.at(commandArr.size()-1).begin() + commandArr.at(commandArr.size()-1).size()-1);
		}
	}
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
			return false;
			perror("exec");
			exit(-1);
		}
	}
	else { //Otherwise, the process is a parent
		waitpid(pid, &status, 0); //wait for the child process to finish
	}	
	return true;
}
bool hasSemi(string s);
bool hasHash(string s);
bool isConnector(string s);

void parse_string(string commandLine, vector<string>&cmdArray) {
	
	char* token; //split command into separate strings
	char* cmd = new char[commandLine.length() + 1];
	strcpy(cmd, commandLine.c_str()); //converts string to char*
	token = strtok(cmd, " "); //tokenize first part of string
	int i = 0;
	for (i = 0; token != NULL; i++) {

		if (token == NULL) { //break out of for loop if there's an empty token
			break;
		}
		string tokenString = string(token);
		//cout << "Token is: " << tokenString << endl;
		if(!isConnector(tokenString)) {
			if (hasSemi(tokenString)) {
				string semicolon = ";";
				tokenString.erase(tokenString.begin() + tokenString.size() -1 );
				cmdArray.push_back(tokenString);
				cmdArray.push_back(semicolon);
			}
			else if(hasHash(tokenString)) {
				string hash = "#";
				tokenString.erase(tokenString.begin());
				cmdArray.push_back(hash);
				cmdArray.push_back(tokenString);
			}
			else{
					cmdArray.push_back(tokenString);
			}
		}
		
		else{	
			cmdArray.push_back(tokenString);
			}
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

bool hasSemi(string s){ //checks for semicolon attached to a word;
	for(int i = 0; i < s.size(); ++i){
		if(s.at(i) == ';'){
				return true; //return position of semi
		}
	}
	return false; // if not found return -1
}

bool hasHash(string s){
	for(int i = 0; i < s.size(); ++i){
			if(s.at(i) == '#') {
					return true;
			}
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
			hasExecuted = execute(command); //executes the first command
			command.clear(); //clear the first command to read the other commands

			//deals with cases when there is two connectors and a command in between
			for (int i = index; i < cmdArr.size(); i++) { //start where we left off, only executes when there is two connectors in between a command
				command.push_back(cmdArr.at(i));
				
				
				if(isConnector(cmdArr.at(i)) && command.size() > 1) { //if there's more commands and the next command is a connector
					if (command.at(0) == "&&") {
						i--; //decrement i to include next connector
						command.erase(command.begin()); //remove connector from command
						command.pop_back(); //remove connector at the end
						if (hasExecuted == true) {
							hasExecuted = execute(command);
						}
						command.clear();
					}
					else if(command.at(0)== ";") {
						i--;
						command.erase(command.begin()); //remove connector from command
						//command.pop_back();
						hasExecuted = execute(command);
						command.clear();
					}
					else if(command.at(0) == "||") {
						i--; //decrement i to include next connector
						command.erase(command.begin()); //delete the connector
						command.pop_back(); //remove connector at end of command
						if (hasExecuted == false) { //if the previous command did not execute, run this command
							hasExecuted = execute(command);
						}
						command.clear();
					}
					else if(command.at(0) == "#" && command.size() == 1) {
						break; //don't do anything if command has comments
					}
				}
			}

			//Below deals with the commands with no connectors (commands at the end)
			if (command.size() != 0 && command.at(0) == "&&") {
				command.erase(command.begin()); //delete connector
				if (hasExecuted == true) {
					hasExecuted = execute(command);
				}
			}
			else if (command.size() != 0 && command.at(0) == "||") {
				command.erase(command.begin());
				if (hasExecuted == false) {
					hasExecuted = execute(command);
				}
			}
			else if (command.size() != 0 && command.at(0) == ";") {
				command.erase(command.begin());
				hasExecuted = execute(command);
			}

			
		}
		return 0;
}

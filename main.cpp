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
		int works = execvp(fullpath,cmdArr); // to check if execvp passes or fails	
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
		string tokenString = string(token);
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

int main () {
		string cmdLine;
		
		while( cmdLine != "exit") {
			vector<string>cmdArr;
			printPrompt();
			getline(cin, cmdLine);
			parse_string(cmdLine, cmdArr);
			execute(cmdArr); //execute the list of commands
		}
		return 0;
}

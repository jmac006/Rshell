#include <iostream>
#include <stdio.h>
#include <unistd.h> //used for fork(), execvp()
#include <sys/types.h> //used for wait
#include <sys/wait.h> //used for wait
#include <string.h>
#include <cstdlib>
using namespace std;



void execute(char** commandArr) {
	
	/*if(line == "exit") {
		exit(0);
	}*/

	int status = 0;	
	char fullpath[20] = "/bin/"; //full file path, starts at bin	
	strcat(fullpath, commandArr[0]); //add the first command to the file path
	pid_t pid = fork();//split the processes into parent and child
	if(pid==0) { //if the process is a child
		execvp(commandArr[0],commandArr);
	}
	else { //Otherwise, the process is a parent
		//wait(NULL);
		waitpid(pid, &status, 0); //wait for the child process to finish
	}

}

void parse_string(string commandLine, char** cmdArray) {//char** is an array of char* 
	char* token; //split command into separate strings
	char* cmd = new char[commandLine.length() + 1];
	strcpy(cmd, commandLine.c_str()); //converts string to char*
	token = strtok(cmd, " "); //tokenize first part of string
	int i = 0;
	for (; token != NULL; i++) { //tokenize rest of string into cmdArray
		cmdArray[i] = token;
		token = strtok(NULL, " ");
	}
	cmdArray[i] = NULL; //set the last value to NULL for execvp
	
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
		char* cmdArr[100]; //will hold an array of parsed commands
		printPrompt();
		string cmdLine = "";
		getline(cin, cmdLine);
		//cout << cmdLine << endl;
		//cout << "First parsed string is: " << cmdArr[0] << endl;
		parse_string(cmdLine, cmdArr); //parses string and populates cmdArr
		execute(cmdArr); //execute the list of commands
		return 0;
}

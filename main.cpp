#include <iostream>
#include <stdio.h>
#include <unistd.h> //used for fork(), execvp()
#include <sys/types.h> //used for wait
#include <sys/wait.h> //used for wait
#include <string.h>
#include <cstdlib>
using namespace std;

void execute(string command) {
	
	pid_t pid = fork(); //creates child process
	int status = 0;
	if (pid == -1) {
		perror("fork");
	}
	if (pid == 0) {
		cout << "Child: " << pid << endl;
		execvp(command.c_str(), NULL); 
	}
	else { //Otherwise, the process is a parent
		cout << "This is a parent process" << endl;
		waitpid(pid, &status, 0); //wait for the child process to finish
		cout << "Process returned" << WEXITSTATUS(status) << endl;
	}
	/*
	if (pid > 0) {
		if (wait(0) == -1) {//waits for child to finish
			perror("wait");
		}
		cout << "Parent: " << pid << endl;
	}*/
}

void parse_string(string &line) {
	if(line == "exit") {
		exit(0);
	}
	char* l = new char[line.length() + 1];
	strcpy(l, line.c_str());
	char* word = strtok(l, "||");
	cout << "Parsed word: " << word << endl;
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
		printPrompt();
		string cmdLine = "";
		getline(cin, cmdLine);
		cout << cmdLine << endl;
		parse_string(cmdLine);
		execute(cmdLine);
		return 0;
}

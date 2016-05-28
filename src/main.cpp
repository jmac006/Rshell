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
	if (commandArr.at(0) == "exit") {
		exit(0);
	}
	if(commandArr.size() > 1){
		if (commandArr.at(1).at(0) == '\"'||commandArr.at(1).at(0) ==  '\'') { //fixes quotation marks when executing echo
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
		if(!isConnector(tokenString)) { //if it a just a connector without any words attached  go straight to push_back
			if (hasSemi(tokenString)) {//if there is a semicolon attached enter loop
				string semicolon = ";";
				tokenString.erase(tokenString.begin() + tokenString.size() -1 ); //remove the semicolon at the end of word
				cmdArray.push_back(tokenString); // push back word without semicolon
				cmdArray.push_back(semicolon);// push back semi colon as its own string
			}
			else if(hasHash(tokenString)) { //if there is a hash enter loop
				string hash = "#";
				tokenString.erase(tokenString.begin()); // remove hashtag located at front of string
				cmdArray.push_back(hash); // push back has as its own string
				cmdArray.push_back(tokenString); // push back word 
			}
			else{ 
					cmdArray.push_back(tokenString); // if it does not contain semi or hash push back word
			}
		}
		else if(hasEParan(tokenString)){
			if(tokenString.size() == 1){
				cmdArray.push_back(tokenString);
			}
			else{
				string paran = "(";
				tokenString.erase(tokenString.begin() + tokenString.size() -1 ); //remove the semicolon at the end of word
				cmdArray.push_back(tokenString); // push back word without semicolon
				cmdArray.push_back(paran);// push back semi colon as its own string
			}
		}
		else if(hasOParan(tokenString)){
			if(tokenString.size() == 1){
				cmdArray.push_back(tokenString);
			}
			else{
				string paran = "(";
				tokenString.erase(tokenString.begin()); // remove hashtag located at front of string
				cmdArray.push_back(paran); // push back has as its own string
				cmdArray.push_back(tokenString); // push back word 
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
	gethostname(host, 1024);
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
	for(unsigned i = 0; i < s.size(); ++i){
		if(s.at(i) == ';'){
				return true; //return position of semi
		}
	}
	return false; // if not found return -1
}
bool hasEParan(string s){ //checks for semicolon attached to a word;

	if(s.at(s.size() - 1) == ')'){
		return true; //return position of semi
	}
	return false; // if not found return -1
}
bool hasOParan(string s){ //checks for semicolon attached to a word;

	if(s.at(0 == '('){
		return true; //return position of semi
	}
	return false; // if not found return -1
}
bool hasHash(string s){
	for(unsigned i = 0; i < s.size(); ++i){
			if(s.at(i) == '#') {
					return true;
			}
	}
	return false;
}

void execCommand(string cmdLine, bool &hasExecuted) {
	vector<string>cmdArr; //holds the whole command line (parsed)
	vector<string>command; //holds individual commands to run in execute() function
	
	if (cmdLine == "exit") {
		exit(0);
	}

	parse_string(cmdLine, cmdArr);

	
	for (unsigned i = 0; i < cmdArr.size(); i++) { //start where we left off, only executes when there is two connectors in between a command
		command.push_back(cmdArr.at(i));

		/*if (hasHash(cmdArr.at(i))) { //if it is a comment, break out of the loop
			break;
		}*/
		if(isConnector(cmdArr.at(i)) && command.size() > 1) { //if there's more commands and the next command is a connector
			if(command.at(1) == "#") { //handles comments after connector
					break;
			}

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
				command.pop_back(); //pop back twice
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
			else if(!isConnector(command.at(0))){
				i--;
				command.pop_back(); //remove the connector
				hasExecuted = execute(command);
				command.clear();
			}
		}
		else if(i == cmdArr.size()-1) { //if this is the last command
			if(isConnector(command.at(command.size()-1))) { //if there is a dangling connector at the end remove it
				command.pop_back();
				break;
			}
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
				break;
			}
			else if (command.size() != 0 && command.at(0) == "#") {
				command.erase(command.begin());
				return;
			}
			else {
				hasExecuted = execute(command);
			}
		}
	}

}

int main () {
	string cmdLine;
	bool hasExecuted = true;	
	while(cmdLine != "exit") {
		printPrompt();
		cmdLine.clear();
		getline(cin, cmdLine);
		if (cmdLine == "exit") { //exits with command "exit"
			exit(0);
		}
		else {
			execCommand(cmdLine,hasExecuted);
		}
	}
	return 0;
}

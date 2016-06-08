#include <iostream>
#include <stdio.h>
#include <unistd.h> //used for fork(), execvp()
#include <sys/types.h> //used for wait
#include <sys/wait.h> //used for wait
#include <sys/stat.h>
#include <string.h>
#include <cstdlib>
#include <fcntl.h> //used for file descriptor
#include <vector>
#include <stack>

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
bool executeTest(vector<string>commandArr){
	struct stat file;
	bool noflag;
	if( commandArr.at(1) != "-e" && commandArr.at(1) != "-f" && commandArr.at(1) != "-d"){
			noflag = true;
	}
	else {
			noflag = false;
	}
	if( commandArr.at(1) == "-e" || noflag ){ //does test -e or test with no specifications
		if( !noflag && stat(commandArr.at(2).c_str(), &file) == 0){
			cout << "(True)" << endl;
			return true;
		}
		else if (noflag && stat(commandArr.at(1).c_str(), &file) ==0){
			cout << "(True)" << endl;
			return true;
		}
		else {
			cout << "(False)" << endl;
			return false;
		}
	}

	else if(commandArr.at(1) == "-f"){	//does test witht -f, so checks if it is a file
		if(stat(commandArr.at(2).c_str(), &file) == 0){
			if(S_ISREG(file.st_mode)){
				cout << "(True)" << endl;
				return true;
			}
			else{
				cout << "(False)" << endl;
				return false;
			}
		}
		else{
			cout << "(False)" << endl;
			return false;
		}
	}

	else if(commandArr.at(1) == "-d"){ // does test with -d, so checks if it a directory
		if(stat(commandArr.at(2).c_str(), &file) == 0) {
			if(S_ISDIR(file.st_mode)){
				cout << "(True)" << endl;
				return true;
			}
			else{
				cout << "(False)" << endl;
				return false;
			}
		}
		else{
			cout << "(False)" << endl;
			return false;
		}
	}
	return false;
}

bool hasOutputRedirect(string s) {
	if(s == ">") {
		return true;
	}
	return false;
}

bool hasAppendRedirect(string s) {
	if(s == ">>") {
		return true;
	}
	return false;
}

bool hasInputRedirect(string s) {
	if(s == "<") {
		return true;
	}
	return false;
}

bool hasPipe(string s) {
	if(s == "|") {
		return true;
	}
	return false;
}

string convertToString(vector<string>command) {
	string s;
	for(unsigned i = 0; i < command.size(); i++) {
		s += command.at(i);
		if(i < command.size()-1) {
			s += " ";
		}
	}
	return s;
}

void execCommand(string cmdLine, bool &hasExecuted);

bool executeRedirect(vector<string>commandArr) {
	//save the stdin and stdout, and use dup to change the stdin/stdout to a file
	int saveSTD[2]; //size of 2 to save stdin and stdout
	vector<string>command; //holds individual commands
	for(unsigned i = 0; i < commandArr.size(); i++) {
		command.push_back(commandArr.at(i));
		if(hasOutputRedirect(commandArr.at(i)) && command.size() > 1) {
			command.pop_back(); //pops the redirection off
			char file[commandArr.at(i+1).size()];
			strcpy(file,commandArr.at(i+1).c_str());
			i++;
			int pfd;
			if((pfd = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP )) == -1) {
				perror("Couldn't open file");
				exit(1);
			}
			saveSTD[1] = dup(1); //saves the stdout to restore later
			dup2(pfd,1); //changed output to the file directory
			string comm = convertToString(command);
			bool didExecute = true;
			execCommand(comm,didExecute);
			dup2(saveSTD[1],1); //restore the stdout
		}
		else if(hasAppendRedirect(commandArr.at(i)) && command.size() > 1) {
			command.pop_back(); //pops the redirection off
			char file[commandArr.at(i+1).size()];
			strcpy(file,commandArr.at(i+1).c_str());
			i++;
			int pfd;
			if((pfd = open(file, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP )) == -1) {
				perror("Couldn't open file");
				exit(1);
			}
			saveSTD[1] = dup(1); //saves the stdout to restore later
			dup2(pfd,1); //changed output to the file directory
			string comm = convertToString(command);
			bool didExecute = true;
			execCommand(comm,didExecute);
			dup2(saveSTD[1],1); //restore the stdout
		}
		else if(hasInputRedirect(commandArr.at(i)) && command.size() > 1) {
			command.pop_back();
			char file[commandArr.at(i+1).size()];
			strcpy(file,commandArr.at(i+1).c_str());
			i++;
			int pfd;
			if((pfd = open(file, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP )) == -1) {
				perror("Couldn't open file");
				exit(1);
			}
			saveSTD[0] = dup(0); //saves the stdin to restore later
			dup2(pfd,0); //changed input to the file directory
			string comm = convertToString(command);
			bool didExecute = true;
			execCommand(comm,didExecute);
			dup2(saveSTD[0],0); //restore the stdin
		}
		/*else if(hasPipe(commandArr.at(i)) && command.size() > 1) {

		}*/
		else {
			return false;
		}
	}
	return true;
}

bool hasSemi(string s);
bool hasHash(string s);
bool isConnector(string s);
bool hasEParen(string s){ //checks for ')'

	if(s.at(s.size() - 1) == ')'){
		return true; //return position of semi
	}
	return false; // if not found return -1
}
bool hasOParen(string s){ //checks for '('

	if(s.at(0) == '(') {
		return true; //return position of semi
	}
	return false; // if not found return -1
}
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
		else if(hasEParen(tokenString)){
			if(tokenString.size() == 1){
				cmdArray.push_back(tokenString);
			}
			else{
				string paran = ")";
				tokenString.erase(tokenString.begin() + tokenString.size() -1 ); //remove the semicolon at the end of word
				cmdArray.push_back(tokenString); // push back word without semicolon
				cmdArray.push_back(paran);// push back semi colon as its own string
			}
		}
		else if(hasOParen(tokenString)){
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

bool hasHash(string s){
	for(unsigned i = 0; i < s.size(); ++i){
			if(s.at(i) == '#') {
					return true;
			}
	}
	return false;
}
bool isTest(string s){ //checks if command is a test case
	if(s == "[" || s == "test"){
			return true;
	}
	return false;
}



bool isRedirect(string s){
	if(s == "|" || s == ">" || s == ">>" || s == "<"){
		return true;
	}
	return false;
}

bool checkRedirect(vector<string>s) {
	for(unsigned i = 0; i < s.size(); i++) {
		if (isRedirect(s.at(i))) {
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

	bool hasRedirect = false;
	
	for (unsigned i = 0; i < cmdArr.size(); i++) { //start where we left off, only executes when there is two connectors in between a command
		command.push_back(cmdArr.at(i));

		/*if (hasHash(cmdArr.at(i))) { //if it is a comment, break out of the loop
			break;
		}*/

		if(isConnector(cmdArr.at(i)) && command.size() > 1) { //if there's more commands and the next command is a connector
			hasRedirect = checkRedirect(command);
			if(command.at(1) == "#") { //handles comments after connector
					break;
			}

			if (command.at(0) == "&&") {
				i--; //decrement i to include next connector
				command.erase(command.begin()); //remove connector from command
				command.pop_back(); //remove connector at the end
				if (hasExecuted == true) {
					if(isTest(command.at(0))){
						hasExecuted = executeTest(command);
					}
					else if(hasRedirect) {
						hasExecuted = executeRedirect(command);
					}
					else{
						hasExecuted = execute(command);
					}
				}
				command.clear();
			}

			else if(command.at(0)== ";") {
				i--;
				command.erase(command.begin()); //remove connector from command
				command.pop_back(); //pop back twice
				if(isTest(command.at(0))){
					hasExecuted = executeTest(command);
				}
				else if(hasRedirect) {
					hasExecuted = executeRedirect(command);
				}
				else{
					hasExecuted = execute(command);
				}
				command.clear();
			}
			else if(command.at(0) == "||") {
				i--; //decrement i to include next connector
				command.erase(command.begin()); //delete the connector
				command.pop_back(); //remove connector at end of command
				if (hasExecuted == false) { //if the previous command did not execute, run this command
					if(isTest(command.at(0))){
						hasExecuted = executeTest(command);
					}
					else if(hasRedirect) {
						hasExecuted = executeRedirect(command);
					}
					else{
						hasExecuted = execute(command);
					}
				}
				else {
					hasExecuted = false;
				}
				command.clear();
			}
			else if(command.at(0) == "#" && command.size() == 1) {

				break; //don't do anything if command has comments
			}
			else if(!isConnector(command.at(0))){ //executes the very first command in cmdArr
				i--;
				command.pop_back(); //remove the connector
				if(isTest(command.at(0))){
					hasExecuted = executeTest(command);
				}
				else if(hasRedirect) {
					hasExecuted = executeRedirect(command);
				}
				else{
					hasExecuted = execute(command);
				}
				command.clear();
			}
			hasRedirect = false;
		}
		else if(i == cmdArr.size()-1) { //if this is the last command
			hasRedirect = checkRedirect(command);
			if(isConnector(command.at(command.size()-1))) { //if there is a dangling connector at the end remove it
				command.pop_back();
				break;
			}
			if (command.size() != 0 && command.at(0) == "&&") {
				command.erase(command.begin()); //delete connector
				if (hasExecuted == true) {
					if(isTest(command.at(0))){
						hasExecuted = executeTest(command);
					}
					else if(hasRedirect) {
						hasExecuted = executeRedirect(command);
					}	
					else{
						hasExecuted = execute(command);
					}
				}
			}
			else if (command.size() != 0 && command.at(0) == "||") {
				command.erase(command.begin());
				if (hasExecuted == false) {
					if(isTest(command.at(0))){
						hasExecuted = executeTest(command);
					}
					else if(hasRedirect) {
						hasExecuted = executeRedirect(command);
					}
					else{
						hasExecuted = execute(command);
					}
				}
			}
			else if (command.size() != 0 && command.at(0) == ";") {
				command.erase(command.begin());
				if(isTest(command.at(0))){
					hasExecuted = executeTest(command);
				}
				else if(hasRedirect) {
					hasExecuted = executeRedirect(command);
				}
				else{
					hasExecuted = execute(command);
				}
				break;
			}
			else if (command.size() != 0 && command.at(0) == "#") {
				command.erase(command.begin());
				return;
			}
			else {
				if(isTest(command.at(0))){
					hasExecuted = executeTest(command);
				}
				else if(hasRedirect) {
					hasExecuted = executeRedirect(command);
				}
				else{
					hasExecuted = execute(command);
				}
			}
		}
	}
}


bool checkParenthesis(string command) {
	stack<char>parenthesis;
	for(unsigned i = 0; i < command.size(); i++) {
		if(command.at(i) == '(') {
			parenthesis.push('(');
		}
		else if(command.at(i) == ')') {
			if(!parenthesis.empty()) {
				parenthesis.pop();
			}
			else {
				cout << "Parenthesis mismatch. Missing \'('." << endl;
				return false;
			}
		}
	}
	if(!parenthesis.empty()) {
		cout << "Parenthesis mismatch. Missing \')'." << endl;
		return false;
	}
	return true;
}

void remove_parenthesis(string &s) {
	for(unsigned i = 0; i < s.size(); i++) {
		if(s.at(i) == '(' || s.at(i) == ')') {
			s.erase(s.begin() + i);
		}
	}
}

void separateParenthesis(string command) { //parses the parenthesis and calls execCommand on those parsed strings
	bool didExecute = true;
	checkParenthesis(command);
	vector<string>parseCommands;
	vector<string>commandList;
	parse_string(command,parseCommands);
	string comm;
	bool insideParenthesis = false;
	for(unsigned i = 0; i < parseCommands.size();i++) {
		comm += parseCommands.at(i);
		if(i < parseCommands.size()-1) { //push individual command if we find a connector and we're not inside the parenthesis
			if(isConnector(parseCommands.at(i+1)) && !insideParenthesis) {
				remove_parenthesis(comm); //remove any parenthesis before pushing individual commands
				commandList.push_back(comm);
				comm.clear();
			}
		}
		if(i < parseCommands.size()-1 && !hasEParen(parseCommands.at(i))) { //won't add a space to the end or after the end parenthesis
			comm += " ";
		}

		if(hasOParen(parseCommands.at(i))) {
			insideParenthesis = true;
		}
		else if(hasEParen(parseCommands.at(i))) {
			remove_parenthesis(comm); //remove any parenthesis before pushing individual commands
			commandList.push_back(comm);
			comm.clear();
			insideParenthesis = false;
		}
		else if(i == parseCommands.size() - 1) {
			remove_parenthesis(comm); //remove any parenthesis before pushing individual commands
			commandList.push_back(comm);
		}
	}
	//cout << "Command size is: " << commandList.size() << endl;
	//cout << "Command List contains: ";

	//execute individual commands based on precedence
	for (unsigned i = 0; i < commandList.size(); i++) {
		execCommand(commandList.at(i),didExecute);
	}

}

int main () {
	string cmdLine;
	//bool hasExecuted = true;	
	while(cmdLine != "exit") {
		printPrompt();
		cmdLine.clear();
		getline(cin, cmdLine);
		if (cmdLine == "exit") { //exits with command "exit"
			exit(0);
		}
		else {
			//execCommand(cmdLine,hasExecuted);
			separateParenthesis(cmdLine);
		}
	}
	return 0;
}

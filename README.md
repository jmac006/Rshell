Assignment 2: RSHELL
====================
Assignment created by Justin Mac, and Satkaran Tamber

Overview
------------
This project is a simple command shell similar to the linux terminal. It is capable of reading single line input commands, including semi-colons, || , and &&. This is the second assignment, assigned by Brian Crites at University of California, Riverside. 

Installation
--------------
1. To install, clone this project at: https://github.com/jmac006/rshell.git. 
2. After cloning, change the directory into rshell (cd rshell). 
3. git checkout hw2 (tag is hw2)
4. Make
5. ./bin/rshell


Bugs/Known Issues/Limitations
------------------------------
* When using semi-colons in a multiple command setting and there is a semi-colon at the end of the command line, it only prints the first command (i.e. [echo a && echo b; ls;] will not work. However [echo a && echo b; ls] does work)
* when using semi-colons, sometimes the exit command must be called multiple times (about 2-3 times) to exit out of the terminal 
* Certain commands like cd are not embedded into the execvp function, so cd will not work on our terminal
* When using semi-colons, user must use a space for the next command after the semi-colon
* When using Hashtags for commenting, user must have a space in front of the hashtag
* Did not use pedantic flag in Makefile since it called a single warning as an error on our array of char pointers in execvp 

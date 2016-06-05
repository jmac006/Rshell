Assignment 4: RSHELL
====================
Assignment created by Justin Mac, and Satkaran Tamber

Overview
------------
This project is a simple command shell similar to the linux terminal. It is capable of reading single line and multiple line input commands, including semi-colons, || , and &&. This command shell is also capable of reading parenthesis. This is the fourth assignment, assigned by Brian Crites at University of California, Riverside. 

Installation
--------------
1. To install, clone this project at: https://github.com/jmac006/rshell.git. 
2. After cloning, change the directory into rshell (cd rshell). 
3. git checkout hw4 (tag is hw4)
4. Make
5. ./bin/rshell


Bugs/Known Issues/Limitations
------------------------------
* Certain commands like cd are not embedded into the execvp function, so cd will not work on our terminal
* When using semi-colons, user must use a space for the next command after the semi-colon
* When using Hashtags for commenting, user must have a space in front of the hashtag
* Did not use pedantic flag in Makefile since it called a single warning on our array of char pointers in execvp
* User is able to use parenthesis, however the user cannot use multiple parenthesis within parenthesis in one command ( ex: ((echo a && echo b) || (echo c)) )
* User cannot use a semi-colon at the end of a parenthesis ( ex: (echo a; echo b;) does not work however (echo a; echo b) does work )

#!/bin/sh

cd ../src/
g++ main.cpp
../bin/./rshell << 'EOF'
ls && test -e Makefile #list and test
[ -e Makefile ] || echo "Makefile not in directory" #test bracket -e flag
[ -d Makefile ] || echo "Not a directory" #test bracket -d flag
[ -f Makefile ] #test bracket -f flag
test -e src || echo "src doesn't exists" #test command with -e flag
test -d src || echo "src isn't a directory" #test command with -d flag
(test -f README && echo "README exists and is a regular file") #test command with -f flag
[ -e src ]; #test command with semi-colon
exit
EOF
printf "\n"

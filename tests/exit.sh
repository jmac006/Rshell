#!/bin/sh
cd ../src
g++ main.cpp
../bin/./rshell << 'EOF'
echo "hello mate" #regular exit
exit
EOF
printf "\n"

../bin/./rshell << 'EOF'
ls && exit #list and exit
EOF
printf "\n"

../bin/./rshell << 'EOF'
ls -l && exit #exit with && operator
EOF
printf "\n"

../bin/./rshell << 'EOF'
(echo a && echo b) && (exit || ls) #echo and exit
EOF
printf "\n"

../bin/./rshell << 'EOF'
ls;
exit || (echo a && echo b) #exit with || operator
EOF
printf "\n"

../bin/./rshell << 'EOF'
[ -e Makefile ];
exit #exit with test command
EOF
printf "\n"

../bin/./rshell << 'EOF'
exit; #exit with semi-colon
EOF
printf "\n"







#!/bin/sh
cd ..
make
./bin/rshell << 'EOF'
ps aux | grep chrome #basic pipe command
ps aux | grep chome && (echo hi > file.txt) #pipe command with > redirection
(echo Hello >> file.txt) && ps aux | grep chrome #pipe command with appending redirection
ls -l | grep src
ls | wc
exit
EOF
printf "\n"
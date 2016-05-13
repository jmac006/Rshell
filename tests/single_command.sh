#!/bin/sh
cd ../src
g++ rshell.cpp
../bin/./rshell << 'EOF'
echo hello
ls
ls -a
echo "how are you doing today"
ls -l
exit
EOF
printf "\n"

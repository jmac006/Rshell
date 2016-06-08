#!/bin/sh
cd ..
make
./bin/rshell << 'EOF'
echo hi > file.txt #basic output redirection
ls -l >> file.txt #output list of directory and appends to file
(echo a && echo b) > file.txt #testing with parenthesis
(echo Hello || echo Bye) >> file.txt #parenthesis with or gates with appending output
cat file.txt >> newfile.txt #copies everything in file to newfile
exit
EOF
printf "\n"
#!/bin/sh

cd ../src/
g++ main.cpp
../bin/./rshell << 'EOF'

(ls && echo a) && (echo b || echo c) # echo "what's your name?"
echo testing #ls || echo  your smelly
ls -a && echo no way jose # should ignore echo jelly beans
ls #-l echo memememe && echo me;
(echo a && echo b) #should echo 
(echo a || echo b) && (echo c || echo d) #testing comment with multiple parenthesis
test -f /test/file/path #should print false
exit
EOF
printf "\n"


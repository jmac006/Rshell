#!/bin/sh
cd ..
make
./bin/rshell << 'EOF'
cat < file.txt
echo hi < file.txt #should just print hi
(echo a && echo b) && (cat < file.txt)

exit
EOF
printf "\n"

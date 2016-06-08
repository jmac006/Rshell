#!/bin/sh
cd ..
make
./bin/rshell << 'EOF'
cat < file.txt
echo hi < file.txt #should just print hi

exit
EOF
printf "\n"
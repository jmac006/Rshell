#!/bin/sh
cd ..
make
./bin/rshell << 'EOF'
(echo A && echo B) || (echo C && echo D)
(echo A || echo B) && (echo C || echo D)
(echo A && echo B || echo C) && (echo D || echo E)
(ls -a && echo hi) || echo hello
(echo A || echo B) && (echo C && echo D) && (echo E && echo F)
(echo A || echo B) && (echo C && echo D) || (echo E && echo F) #testing long parenthesis
(echo a; echo b) #testing parenthesis with semi-colon
(echo a; echo b) && (echo c || echo d) #testing combination of semi-colons and connectors
exit
EOF
printf "\n"
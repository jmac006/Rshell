COMPILE = g++
FLAGS = -Werror -ansi

all: clean
	mkdir ./bin
	$(COMPILE) $(FLAGS) ./src/main.cpp -o ./bin/rshell

rshell: clean
	mkdir ./bin
	$(COMPILE) $(FLAGS) ./src/main.cpp -o ./bin/rshell

clean: 
	rm -rf ./bin

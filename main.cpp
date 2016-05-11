#include <iostream>
#include <stdio.h>
#include <unistd.h> //used for fork(), exec()
#include <sys/types.h> //used for wait
#include <sys/wait.h> //used for wait

using namespace std;

void processes() {
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
	}
	if (pid == 0) {
		cout << "Child: " << pid << endl;
	}
	if (pid > 0) {
		/*if (wait(0) == -1) {
			perror("wait");
		}*/
		cout << "Parent: " << pid << endl;
	}

}

int main () {
		processes();
		return 0;
}

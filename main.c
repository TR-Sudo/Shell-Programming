/*******************************************
 *
 *                  main.c
 *
 *  The main procedure for the mysh program.
 ******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mysh.h"

int main(int argc, char **argv) {
	struct Pipeline *pipe;
	struct stat status;
	
	/*
	 *  Create the initial prompt string
	 *  Determine whether we are connected to a terminal
	 */
	Prompt = (char*) malloc(2);
	strcpy(Prompt, "?");
	fstat(0,&status);

	/*
	 *  Loop parsing one line at a time and executing it
	 */
	while (1) {
		if (S_ISCHR(status.st_mode))
			printf("%s ", Prompt);
		pipe = parse(stdin);
		if (pipe == NULL)
			break;
		//exit code
		if (strcmp(pipe->commands->name, "exit") == 0) {
			break;
		}
		//prompt change code
		if (strcmp(pipe->commands->name, "prompt") == 0) {
			struct Arg* cur = pipe->commands->args;
			strcpy(Prompt, cur->name);
			free(cur);
		}
		if (exec(pipe))
			break;
	}
	free(Prompt);
}

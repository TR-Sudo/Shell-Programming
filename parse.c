/*****************************************************
 *
 *              parse.c
 *
 *  Parse a pipeline.  This is a fairly simple
 *  recursive decent parser.  The parse() function
 *  returns a Pipeline that represents the parsed
 *  command line.  The description of the Pipeline
 *  data structure is in the mysh.h file.
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mysh.h"

/* 
*   The term function returns 1 (true) if the character c
 *  signals the end of a command name or parameter.
 */
int term(char c) {
	if(c == ' ')
		return(1);
	if(c == '>')
		return(1);
	if(c == '<')
		return(1);
	if(c == '|')
		return(1);
	if(c == '\n')
		return(1);
	return(0);
}

struct Pipeline *parse(FILE *fin) {
	char buffer[512];		// input buffer
	char *ret;
	int i;				// current position in the input buffer
	int j;
	struct Pipeline *pipe;		// the parsed command line
	struct Command *last;		// the previous command in the pipe
	struct Command *command;	// the current command in the pipe
	int len;
	struct Arg *arg;		// the current argument to command
	struct Arg *argLast;		// the previous argument to the command

	/*
	 *  Read the next line of input and check for end of file
	 */
	ret = fgets(buffer, 512, fin);
	if(ret == NULL)  {
		if(errno != 0)
			printf("error: %s\n",strerror(errno));
		return(0);
	}
	pipe = 0;
	last = 0;
	i = 0;
	/*
	 *  Process characters until we reach the end of the input buffer
	 */
	while(buffer[i] != '\0') {
		// check for end of line
		if(buffer[i] == '\n') {
			// if empty line call self recursively
			if(pipe != 0)
				return(pipe);
			else
				return(parse(fin));
		}
		// check for comments
		if(buffer[i] == '#') {
			// rest of the line is a comment
			if(pipe != 0) {
				return(pipe);
			// comment line, call self recursively
			} else {
				return(parse(fin));
			}
		}
		// start looking for a command, j marks the start of the command
		j=i;
		while(!term(buffer[i])) i++;
		// can't find a command
		if(i == j) {
			// must be at the end of the line, return parsed command line
			if(pipe != 0) 
				return(pipe);
			// must be a blank line, call self recursively
			else
				return(parse(fin));
		}
		//  if this is the start of the command line, create a new Pipeline
		if(pipe == 0) {
			pipe = (struct Pipeline*)malloc((sizeof *pipe));
		}
		//  create a new Command struct for the command
		command = (struct Command*)malloc(sizeof(*command));
		// check to see if this is the first command on the line
		// otherwise, link it into the command list
		if(last == 0) {
			pipe->commands = command;
			last = command;
		} else {
			last->next = command;
			last = command;
		}
		// copy the command name
		len = i-j+1;
		command->name = malloc(len);
		strncpy(command->name, &buffer[j], len-1);
		command->name[len-1] = '\0';
		// initialize the command structure
		command->input = NULL;
		command->output = NULL;
		command->args = NULL;
		command->next = NULL;
		//  skip over any blanks between the command name and arguements
		while(buffer[i] == ' ') i++;
		//  start assembling the arguments
		argLast = 0;
		while(!term(buffer[i])) {
			// allocate the argument structure
			arg = (struct Arg*)malloc(sizeof(*arg));
			// check to see if this is the first argument
			// otherwise, link it into the arguement list
			if(argLast == 0) {
				command->args = arg;
				argLast = arg;
			} else {
				argLast->next = arg;
				argLast = arg;
			}
			//  look for the argument, a name just like a command
			j = i;
			while(!term(buffer[i])) i++;
			len = i-j+1;
			// allocate memory for the argument, and add it to the
			// argument structure
			arg->name = (char*)malloc(len);
			strncpy(arg->name,&buffer[j], len-1);
			arg->name[len-1] = '\0';
			arg->next = NULL;
			while(buffer[i] == ' ') i++;
		}
		//  again, skip any possible blanks
		while(buffer[i] == ' ') i++;
		//  look for input redirection
		if(buffer[i] == '<' ) {
			// look for the file name
			while(term(buffer[i])) i++;
			j = i;
			while(!term(buffer[i])) i++;
			len = i-j+1;
			//  add the file name to the Command structure
			command->input = (char*) malloc(len);
			strncpy(command->input, &buffer[j], len-1);
			command->input[len-1] = '\0';
		}
		//  look for output redirection
		while(buffer[i] == ' ') i++;
		if(buffer[i] == '>') {
			//  look for the file name
			while(term(buffer[i])) i++;
			j = i;
			while(!term(buffer[i])) i++;
			len = i-j+1;
			//  add the file name to the Command structure
			command->output = (char*) malloc(len);
			strncpy(command->output, &buffer[j], len-1);
			command->output[len-1] = '\0';
		}
		//  again, skip over blanks
		while(buffer[i] == ' ') i++;
		//  check for a pipe symbol, if not we are done
		if(buffer[i] != '|') {
			return(pipe);
		}
		// make sure to skip over the pipe symbol and any blanks
		i++;
		while(buffer[i] == ' ') i++;
	}
	return(pipe);
}

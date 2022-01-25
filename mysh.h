/**************************************
 *
 *                mysh.h
 *
 *  Common definitions for mysh
 *************************************/

/*
 *  Linked list of command arguments
 */
struct Arg {
	char *name;		// argument
	struct Arg *next;	// next argument
};

/*
 *  Linked list of commands in a pipeline
 */
struct Command {
	char *name;		// command name
	char *input;		// input redirection
	char *output;		// outpur redirection
	struct Arg *args;	// argument list
	struct Command *next;	// next command in pipeline
};

/*
 *  Command pipeline
 */
struct Pipeline {
	struct Command *commands;	// first command in pipeline
};

/*
 *  Prompt string for shell
 */
char *Prompt;

struct Pipeline *parse(FILE *fin);
int exec(struct Pipeline *pipe);

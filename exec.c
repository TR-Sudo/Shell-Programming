/**************************************************
 *
 *                   exec.c
 *
 *  The exec function that executes a command
 *  pipeline.
 ***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mysh.h"

/*
 *  Execute a pipeline
 */

int exec(struct Pipeline *line) {
    int count = 0;
    struct Arg* cur = line->commands->args;
    while (cur != NULL) {// counts number of commands
        cur = cur->next;
        count += 1;
    }


    char* argv[count + 1];
    struct Arg* ncur = line->commands->args;
    argv[0] = line->commands->name;
    int nc = 1;
    while (ncur != NULL) {
        argv[nc] = ncur->name;
        ncur = ncur->next;
        nc += 1;
    }
    argv[count + 1] = NULL;// assigned values to a argv array

    //PIPELINE
    struct Command* pipe2 = line->commands;
    int pipes = 0;// count of pipes required
    int newcount = 0;
    while (pipe2 != NULL) {
        if (line->commands != NULL) {
            pipes += 1;
        }
        pipe2 = pipe2->next;
        newcount += 1;
    }

    struct Command* pipe3 = line->commands;
    char* newargv[newcount + 1];
    int n2c = 0;
    while (pipe3 != NULL) {
        newargv[n2c] = pipe3->name;
        pipe3 = pipe3->next;
        n2c += 1;
    }
    newargv[newcount] = NULL;// assigned pipes values to new argv
    //PIPELINE

    if (count >= 0 && line->commands->name != (char*)"exit" && pipes == 1) {//will only work if count above 0 and no pipes
        int pid = fork();
        if (pid < 0) {
            perror("Fork Failed");
        }
        if (line->commands->input == NULL && line->commands->output == NULL) {//only commands like ls,w
            if (pid == 0) {
                execvp(argv[0], argv);
            }
        }
        else if (line->commands->output != NULL) {// output command output to file
            if (pid == 0) {
                int outFile = open(line->commands->output, O_WRONLY | O_CREAT, 0777);//create file to write write only with access 0777
                dup2(outFile, STDOUT_FILENO);//replace standard out to outputfile
                execvp(argv[0], argv);//run command close file
                close(outFile);
            }
        }
        else if (line->commands->input != NULL) {// for one argument
            if (pid == 0) {
                if (access(line->commands->input, F_OK) == 0) {//read file and check if it exits
                    char* newargv[] = { argv[0],line->commands->input,NULL };
                    execvp(newargv[0], newargv);
                }
                else {
                    printf("File does not exis\n");
                }
            }
        }
    }
    ///////////// Pipeline
    if (pipes > 1 && line->commands->name != (char*)"exit") {
        int pipeid[pipes];
        pipe(pipeid);
        if (pipes == 2) {// works for 2 pipes
            int pid2 = fork();//new fork
            if (pid2 < 0) {
                perror("Fork Failed");
            }
            if (pid2 == 0) {
                dup2(pipeid[1], STDOUT_FILENO);
                execlp(newargv[0], newargv[0], (char*)NULL);
            }
            pid2 = fork();
            if (pid2 < 0) {
                perror("Fork Failed");
            }
            if (pid2 == 0) {
                dup2(pipeid[0], STDIN_FILENO);
                close(pipeid[1]);
                execlp(newargv[1], newargv[1], (char*)NULL);
            }
            close(pipeid[1]);
            close(pipeid[0]);
        }
    }
    //PIPELINE
    wait(NULL);// for all child
    return(0);
}

#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h> // for PATH_MAX
#include <string.h>
#include "LineParser.h"
#include <stdlib.h> //for exiting with error


void execute(cmdLine *pCmdLine, int debug_mode) {
    int pid = fork();

    if (pid == 0) {
        // We use _exit() instead of exit() so the child process terminates immediately without flushing the parent's I/O buffers or accidentally continuing as a duplicate shell.
        //the child procces
        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
            perror("Error executing command");
            _exit(1); // end the child procces in case of an error
        }
    } else if (pid > 0) {
        // main procces (the shell itself)
        if (debug_mode) {
            fprintf(stderr, "Procces ID: %d\n", pid);
            fprintf(stderr, "Executing command: %s\n", pCmdLine->arguments[0]);
            //in foreground blocking is '1', for background blocking is '0'
            fprintf(stderr, "Foreground or Background: %s\n", pCmdLine->blocking ? "Foreground" : "Background");
        }
    } else {
        //Error in creating the fork
        perror("fork failed");
    }
}

int main(int argc, char **argv) {
    char cwd[PATH_MAX];
    char input [2048];

    int debug_mode = 0;
    for (int i = 1; !debug_mode && i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) { //If -d was recieved as argument
             debug_mode = 1;
        }
    }
    while(1) {
        // gets the current path
        if (getcwd(cwd, PATH_MAX) != NULL) {
            // print path to the screen
            printf("%s> ", cwd);
        } else {
            perror("getcwd error");
        }
        
          // read input from user
        if (fgets(input, 2048, stdin) == NULL) {
            break; // in case of error or end of file
        }

        // check if user want to quit
         //we use strncmp to check only first 4 chars (to not include new line for example)
        if (strncmp(input, "quit", 4) == 0) {
            break; // break the loop and exit the program
        }
        //Turn the input into a cmdLine structure
        cmdLine *parsedLine = parseCmdLines(input);

        execute(parsedLine, debug_mode);
        freeCmdLines(parsedLine); //Free allocated memory

    }
    
    return 0;
}


#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h> // for PATH_MAX
#include <string.h>
#include "LineParser.h"
#include <stdlib.h> //for exiting with error

void execute(cmdLine *pCmdLine) {
    // run the command via 'execv'
    //arguments[0] is the name of the command (and '->arguments' is the arguments array)
    if (execv(pCmdLine->arguments[0], pCmdLine->arguments) == -1) { 
        perror("Error executing command"); // print error in case of failure
        exit(1); //exit with error
    }
}

int main(int argc, char **argv) {
    char cwd[PATH_MAX];
    char input [2048];

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

        execute(parsedLine);
        freeCmdLines(parsedLine); //Free allocated memory

    }
    
    return 0;
}


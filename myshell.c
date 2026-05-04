#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h> // for PATH_MAX
#include <string.h>
#include "LineParser.h"
#include <stdlib.h> //for exiting with error
#include <sys/wait.h> //for wait
#include <signal.h>
#include <fcntl.h>


void execute(cmdLine *pCmdLine, int debug_mode) {
    //Send a SIGSTOP signal to a process to make it "sleep".
    if(strcmp(pCmdLine->arguments[0], "stop") == 0) {
        if(pCmdLine->argCount < 2) {
            fprintf(stderr, "missing process ID argument.\n");
        } else {
            int pid = atoi(pCmdLine->arguments[1]);
            if (kill(pid, SIGSTOP) == -1) {
                perror("Error sending SIGSTOP signal");
            }
        }
        return;
    }
    
    //Wake up a sleeping (stopped) process (SIGCONT).
    if(strcmp(pCmdLine->arguments[0], "wakeup") == 0) {
        if(pCmdLine->argCount < 2) {
            fprintf(stderr, "missing process ID argument.\n");
        } else {
            int pid = atoi(pCmdLine->arguments[1]);
            if (kill(pid, SIGCONT) == -1) {
                perror("Error sending SIGCONT signal");
            }
        }
        return;
    }
    
    //Terminate a running/sleeping process (SIGINT).
    if(strcmp(pCmdLine->arguments[0], "ice") == 0) {
        if(pCmdLine->argCount < 2) {
            fprintf(stderr, "missing process ID argument.\n");
        } else {
            int pid = atoi(pCmdLine->arguments[1]);
            if (kill(pid, SIGINT) == -1) {
                perror("Error sending SIGINT signal");
            }
        }
        return;
    }
    
    //Force termination of an entire process group (SIGKILL to the process group headed by ID). 
    if(strcmp(pCmdLine->arguments[0], "nuke") == 0) {
        if(pCmdLine->argCount < 2) {
            fprintf(stderr, "missing process ID argument.\n");
        } else {
            int pid = atoi(pCmdLine->arguments[1]);
            // "killpg" sends the sig to the process group instead of a single process
            if (killpg(pid, SIGKILL) == -1) {
                perror("Error sending SIGKILL signal");
            }
        }
        return;
    }

    //"cd" requires special treatment
    if (strcmp(pCmdLine->arguments[0], "cd") == 0){
        if (chdir(pCmdLine->arguments[1]) != 0) {
            perror("chdir() failed");
        }
        return;
    }
    
    int pid = fork();
    //Child procces
    if (pid == 0) {
        //the child procces
        // redirect input
        if (pCmdLine->inputRedirect != NULL) {
            close(0); // close the normal input (keyboard)
            
            //Open the file. Because 0 is free now, the file will automatically get the symbol 0
            if (open(pCmdLine->inputRedirect, O_RDONLY) == -1) {
                perror("Error opening input file");
                _exit(1);
            }
        }

        // Output Redirection
        if (pCmdLine->outputRedirect != NULL) {
            close(1); //close the normal output channel (the screen)
            //Open the file. Because 1 is free now, the file will automatically get the symbol "1"
            /*Explanation for the flags:
            O_WRONLY: open the file Write only (no need to read)
            O_CREAT: create the file if it does'nt exist yet. "0644" is the premissions for the created file
            O_TRUNC: remove the content of the file (revert it to size 0)
            */

            if (open(pCmdLine->outputRedirect, O_WRONLY | O_CREAT | O_TRUNC, 0644) == -1) {
                perror("Error opening output file");
                _exit(1);
            }
        }

        if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
            perror("Error executing command");
                    
            // We use _exit() instead of exit() so the child process terminates immediately without flushing the 
            //parent's I/O buffers or accidentally continuing as a duplicate shell.
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
        //task 1c
        if(pCmdLine->blocking){
            waitpid(pid, NULL, 0);
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
        if(parsedLine){
            execute(parsedLine, debug_mode);

        }
        freeCmdLines(parsedLine); //Free allocated memory
    }
    
    return 0;
}


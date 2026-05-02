#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h> // for PATH_MAX

int main(int argc, char **argv) {
    char cwd[PATH_MAX];

    while(1) {
        // gets the current path
        if (getcwd(cwd, PATH_MAX) != NULL) {
            // print path to the screen
            printf("%s> ", cwd);
        } else {
            perror("getcwd error");
        }
        
        // Here will be the code for next parts
    }
    
    return 0;
}
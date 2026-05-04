#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "missing <message>\n");
        return 1;
    }
    
    int pipefwd[2];  // [0] = read end, [1] = write end
    
    // Create the pipe
    if (pipe(pipefwd) == -1) {
        perror("pipe creation failed");
        return 1;
    }
    
    int pid = fork();
    
    if (pid == 0) {
        // Child process: read from pipe
        close(pipefwd[1]);  // Close write end in child
        
        char buffer[256];
        // store num bytes processed
        int bytes_read = read(pipefwd[0], buffer, sizeof(buffer));
        // insert null terminator at the end of the string
        if (bytes_read == -1) {
            perror("read failed");
            return 1;
        }
        buffer[bytes_read] = '\0';


        // Print the received message
        printf("%s\n", buffer);
        
        close(pipefwd[0]);  // Close read end
        return 0;
    } 
    else if (pid > 0) {
        // Parent process: write to pipe
        close(pipefwd[0]);  // Close read end in parent
        
        // write into pipefwd[1] strlen amount of bytes from argv[1]
        write(pipefwd[1], argv[1], strlen(argv[1]));

        // close the pipe
        close(pipefwd[1]);

        wait(NULL);  // Wait for child
        return 0;
    } 
    else {
        perror("fork creation failed");
        return 1;
    }
}
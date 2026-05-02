#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <string.h>

void handler(int sig) {
    printf("\nRecieved Signal : %s\n", strsignal(sig));
    
    if (sig == SIGTSTP) {
        signal(SIGTSTP, SIG_DFL);
    } else if (sig == SIGCONT) {
        signal(SIGCONT, SIG_DFL);
    }
    
    signal(sig, SIG_DFL);
    raise(sig); //Re-send the signal, but this time for the OS to take care of it (for example - stop the program)

    /*
    We need to register the opposite signal handler.
    Because we just changed the current signal to default (SIG_DFL) so the OS will actually stop/wake the process.
    If we just stopped (SIGTSTP), we need our custom handler to catch the wake up call (SIGCONT) later.
    If we just woke up (SIGCONT), we need our custom handler to be ready to catch the next stop call (SIGTSTP).
    */

    if (sig == SIGTSTP) {
        signal(SIGCONT, handler);
    }
    if (sig == SIGCONT) {
        signal(SIGTSTP, handler);
    }
}

int main(int argc, char **argv) {
    //Registering our function to take care of the required signals
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    signal(SIGCONT, handler);

    //Infinite loop so our program stays alive waiting for signals
    while(1) {
        sleep(2); 
    }
    
    return 0;
}
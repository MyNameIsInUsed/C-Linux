#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MAX_CHILD 3 // Max number child processes

void sigint_handler(int sig) {
    printf("\nInvalid command enter by the user...\n");
    exit(0);
}

int main() {
    int child_pids[MAX_CHILD];
    int child_num = 0;
    int pipefds[2]; 
    char buffer[50]; // buffer

    signal(SIGINT, sigint_handler); // call interrupt handler

    // pipe
    if (pipe(pipefds) == -1) {
        perror("FAILED");
        exit(EXIT_FAILURE);
    }

    printf("Siapa nama awak? : ");
    fgets(buffer, sizeof(buffer), stdin); 

    // Fork 
    for (int i = 0; i < MAX_CHILD; i++) {
        int pid = fork();

        if (pid == -1) {
            perror("FAILED");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child 
            close(pipefds[1]); 
            char child_msg[50]; 
            int bytes = read(pipefds[0], child_msg, sizeof(child_msg)); 

            printf("Child process PID %d received name: %.*s\n", getpid(), bytes, child_msg);
            close(pipefds[0]); 
            exit(0);
            
        } else {
            // Parent 
            child_pids[i] = pid;
            child_num++;
        }
    }

    // Parent 
    close(pipefds[0]); 

    
    for (int i = 0; i < child_num; i++) {
        write(pipefds[1], buffer, sizeof(buffer));
    }

    close(pipefds[1]); 

    // child terminate
    for (int i = 0; i < child_num; i++) {
        waitpid(child_pids[i], NULL, 0);
    }

    return 0;
}

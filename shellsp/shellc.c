#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_CMD_LENGTH 1024

int main() {
    char command[MAX_CMD_LENGTH];
    char *args[MAX_CMD_LENGTH / 2 + 1]; 
    while (1) {
        
        printf("shellc> ");
        fflush(stdout);
        // Read command from user
        if (fgets(command, sizeof(command), stdin) == NULL) {
            perror("fgets failed");
            continue; // Exit on EOF
        }
        command[strlen(command) - 1] = '\0'; // Remove newline character
        if (strcmp(command,"exit") == 0) {
            printf("Exiting shell...\n");
            exit(0); // Exit the shell
        }

        // Tokenize command
        char *token = strtok(command, " ");
        int i;
        for ( i = 0; token != NULL && i < MAX_CMD_LENGTH / 2; i++) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Null-terminate the args array

        // Create child process
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            continue;
        } else if (pid == 0) {

            // Child process executes the command
            if (execvp(args[0], args) < 0) {
                perror("Execution failed");
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent process waits for child to finish
            wait(NULL);  
        }
    }

    return 0;
}
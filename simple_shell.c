#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64
#define PROMPT "$ "

// Prototypes de fonctions
void display_prompt();
char *read_input();
char **parse_input(char *input);
int execute_command(char **args);
int is_builtin(char **args);
void execute_builtin(char **args);
void shell_exit();
int shell_cd(char **args);
int shell_help(char **args);
int shell_pwd(char **args);

// Liste des commandes intégrées
char *builtin_commands[] = {
    "cd",
    "exit",
    "help",
    "pwd"
};

// Fonctions correspondantes aux commandes intégrées
int (*builtin_functions[]) (char **) = {
    &shell_cd,
    NULL, // exit est géré séparément
    &shell_help,
    &shell_pwd
};

int num_builtins() {
    return sizeof(builtin_commands) / sizeof(char *);
}

int main() {
    printf("=== Simple Shell ===\n");
    printf("Type 'help' for available commands\n");
    
    while (1) {
        display_prompt();
        
        char *input = read_input();
        if (!input) {
            continue;
        }
        
        char **args = parse_input(input);
        if (!args[0]) {
            free(input);
            free(args);
            continue;
        }
        
        // Vérifier si c'est une commande exit
        if (strcmp(args[0], "exit") == 0) {
            free(input);
            free(args);
            shell_exit();
        }
        
        // Exécuter la commande
        execute_command(args);
        
        // Libérer la mémoire
        free(input);
        free(args);
    }
    
    return 0;
}

void display_prompt() {
    printf(PROMPT);
    fflush(stdout);
}

char *read_input() {
    char *input = NULL;
    size_t bufsize = 0;
    ssize_t characters;
    
    characters = getline(&input, &bufsize, stdin);
    
    if (characters == -1) {
        if (feof(stdin)) {
            printf("\n");
            exit(0);
        } else {
            perror("getline");
            return NULL;
        }
    }
    
    // Supprimer le saut de ligne
    input[strcspn(input, "\n")] = '\0';
    
    return input;
}

char **parse_input(char *input) {
    char **args = malloc(MAX_ARGS * sizeof(char *));
    if (!args) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    
    char *token;
    int position = 0;
    
    token = strtok(input, " ");
    while (token != NULL && position < MAX_ARGS - 1) {
        args[position] = token;
        position++;
        token = strtok(NULL, " ");
    }
    
    args[position] = NULL;
    return args;
}

int execute_command(char **args) {
    // Vérifier si c'est une commande intégrée
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_commands[i]) == 0) {
            if (builtin_functions[i]) {
                return (*builtin_functions[i])(args);
            }
        }
    }
    
    // Pour les commandes externes
    pid_t pid = fork();
    
    if (pid == 0) {
        // Processus enfant
        if (execvp(args[0], args) == -1) {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Erreur de fork
        perror("fork");
    } else {
        // Processus parent
        int status;
        waitpid(pid, &status, 0);
    }
    
    return 1;
}

// Implémentation des commandes intégrées

void shell_exit() {
    printf("Goodbye!\n");
    exit(0);
}

int shell_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "shell: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("shell");
        }
    }
    return 1;
}

int shell_help(char **args) {
    (void)args; // Éviter l'avertissement unused parameter
    printf("Simple Shell - Available Commands:\n");
    printf("  cd <directory>     - Change directory\n");
    printf("  exit               - Exit the shell\n");
    printf("  help               - Show this help message\n");
    printf("  pwd                - Print working directory\n");
    printf("  [external command] - Execute system commands\n");
    printf("\nExamples:\n");
    printf("  $ ls -la\n");
    printf("  $ cd /home/user\n");
    printf("  $ pwd\n");
    return 1;
}

int shell_pwd(char **args) {
    (void)args; // Éviter l'avertissement unused parameter
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd");
    }
    return 1;
}

int is_builtin(char **args) {
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_commands[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void execute_builtin(char **args) {
    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_commands[i]) == 0) {
            if (builtin_functions[i]) {
                (*builtin_functions[i])(args);
            }
        }
    }
}
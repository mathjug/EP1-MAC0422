#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define TRUE 1
#define MAX_SIZE 200 /* maximum size of the user's input */

void protection_000(char* address) {
    /*
    Function that implements the command "nem_eu_nem_de_ninguem".
    Changes the permissions of the file passed as argument to 000.
    */
    int mode = strtol("0000", 0, 8);
    if (chmod(address, mode) == 0)
        printf("Success :)\n");
    else
        printf("Fail :(\n");
    return;
}

void protection_700(char* address) {
    /*
    Function that implements the command "soh_eumesmo".
    Changes the permissions of the file passed as argument to 700.
    */
    int mode = strtol("0700", 0, 8);
    if (chmod(address, mode) == 0)
        printf("Success :)\n");
    else
        printf("Fail :(\n");
    return;
}

int run_and_see(char* address) {
    /*
    Function that implements the command "rodaeolhe".
    Starts the execution of the file passed as argument and waits until it ends.
    */
    pid_t pid = fork();
    int status = 0;
    char *args[2];

    if (pid == 0) { /* child process */
        args[0] = address;
        args[1] = NULL;
        execve(address, args, NULL);
        perror("execve");
        exit(EXIT_FAILURE);
    }

    else if (pid > 0) { /* parent process */
        waitpid(pid, &status, 0);
        printf("Program %s exited with status %d\n", address, status);
    }

    else { /* fork failed */
        perror("fork");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int run_in_background(char* address) {
    /*
    Function that implements the command "sohroda".
    Starts the background execution of the file passed as argument.
    */
    int fd[2];
    pid_t pid = fork();
    char *args[2];

    /* create the pipe (to close the access of the child to stdin) */
    if (pipe(&fd[0]) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) { /* child process */
        close(fd[0]);
        dup2(fd[1], STDIN_FILENO); /* redirect stdin to the write end of the pipe */
        close(fd[1]);

        args[0] = address;
        args[1] = NULL;
        execve(address, args, NULL);
        perror("execve");
        exit(EXIT_FAILURE);
    }
    
    else if (pid < 0) { /* fork failed */
        perror("fork");
        exit(EXIT_FAILURE);
    }

    else { /* parent process */
        close(fd[1]);
        close(fd[0]);
    }

    return 0;
}

void shell_loop() {
    /*
    Implements the simplified shell, receiving user input and calling the commands.
    */
    int size, counter1, counter2, sign, i;
    char* word1;
    char* word2;
    char input[MAX_SIZE];

    while (TRUE) {
        printf(">> ");

        fgets(input, MAX_SIZE, stdin);
        input[strcspn(input, "\n")] = 0; /* remove '\n' */

        size = strlen(input);
        word1 = calloc(size + 1, sizeof(char)); /* stores the command */
        counter1 = 0;
        word2 = calloc(size + 1, sizeof(char)); /* stores the file path */
        counter2 = 0;

        if (!word1 || !word2)
            exit(-1);

        /* ---- THE FOLLOWING LINES ALLOW THE READABILITY OF PATHS THAT INCLUDE WHITESPACES ---- */

        sign = 0; /* sign to indicate that a first whitespace was already found */
        
        for (i = 0; i < strlen(input); i++) { /* iterating over the characters of the command */
            if (input[i] == ' ' && sign == 0)
                sign = 1; /* found the first whitespace */
            else if (sign == 0)
                word1[counter1++] = input[i];
            else if (sign == 1) /* accepts cases in which there are whitespaces in the path */
                word2[counter2++] = input[i];
        }

        word1[counter1] = '\0';
        word2[counter2] = '\0';

        /* ------------------------------------------------------------------------------------- */

        if (!strcmp(word1, "quit"))
            exit(0);
        else if (!strcmp(word1, "nem_eu_nem_de_ninguem"))
            protection_000(word2);
        else if (!strcmp(word1, "soh_eumesmo"))
            protection_700(word2);
        else if (!strcmp(word1, "rodaeolhe"))
            run_and_see(word2);
        else if (!strcmp(word1, "sohroda"))
            run_in_background(word2);

        free(word1);
        free(word2);
        continue;
    }
    
    return;
}

int main() {
    printf("To leave the Mini Shell, type 'quit'.\n\n");
    shell_loop();
    return 0;
}
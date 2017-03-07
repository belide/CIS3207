/*
 * =====================================================================================
 *
 *       Filename:  shell.c
 *
 *    Description:  Simple bash shell made for CIS3207
 *
 *        Version:  1.0
 *        Created:  03/01/2017 04:54:19
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tu V.T. Tran (tuvttran), tuvtran97@gmail.com
 *        Company:  Temple University
 *
 * =====================================================================================
 */

#define _GNU_SOURCE
#define _XOPEN_SOURCE

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "shell.h"

// to print current working directory
char *prompt;

// list of built-in commands
char *builtin_cmd[] = {
    "cd",
    "help",
    "quit",
    "echo",
    "export",
    "environ"
};

// array of pointer to function that takes char ** as input and return int
int (*builtin_func[]) (char **) = {
    &sh_cd,
    &sh_help,
    &sh_quit,
    &sh_echo,
    &sh_export,
    &sh_environ
};

int main(int argc, char **argv) {

    // running commang loop
    shell_loop();

    return EXIT_SUCCESS;
}

void shell_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        // this is the current working directory
        prompt = getcwd(prompt, BUF_SIZE);
        printf("%s>", prompt);
        line = read_line();
        args = split_line(line);
        status = sh_execute(args);

        free(line);
        free(args);
    } while(status);
}

int num_builtins() {
    return sizeof(builtin_cmd) / sizeof(char *);
}

// read the line and return a pointer to the beginning of the string
char *read_line(void) {
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, stdin);
    return line;
}

// split the line to extract command name and arguments
char **split_line(char *line) {
    char **tokens;
    char *token;
    int pos = 0;

    // allocate an array of string tokens
    if ((tokens = malloc(sizeof(char*) * TOK_BUFSIZE)) == NULL)
        exit(EXIT_FAILURE);

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[pos++] = token;
        token = strtok(NULL, TOK_DELIM);
    }

    tokens[pos] = NULL;
    return tokens;
}

// process launch
int proc_launch(char **args) {
    pid_t pid, wpid;
    int status;

    int bg = FALSE;
    char proc_name[strlen(args[0])];
    memset(proc_name, '\0', sizeof(proc_name));

    // check if there is '&' indicating background process
    if (args[0][strlen(args[0]) - 1] == '&') {
        strncpy(proc_name, args[0], strlen(args[0]) - 1);
        bg = TRUE;
    }

    pid = Fork();
    if (pid == 0) {
        // child process
        if (execvp(proc_name, args) < 0) {
            // error exec-ing
            fprintf(stderr, "shell: command not found: %s", args[0]);
            exit(EXIT_FAILURE);
        } else {
            // parent process
            // don't wait if it's a background process
            if (bg == FALSE) {
                do {
                    wpid = waitpid(pid, &status, WUNTRACED);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            } else
                return 1;
        }
    }

    return 1;
}

// shell execute and return a status
int sh_execute(char **args) {
    if (args[0] == NULL) {
        // empty command
        return 1;
    }

    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_cmd[i]) == 0) {
            // this is a built in command
            return (*builtin_func[i])(args);
        }
    }

    return proc_launch(args);
}
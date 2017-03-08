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
    "clr",
    "echo",
    "environ",
    "export",
    "help",
    "pause",
    "quit"
};

// array of pointer to function that takes char ** as input and return int
int (*builtin_func[]) (char **) = {
    &sh_cd,
    &sh_clr,
    &sh_echo,
    &sh_environ,
    &sh_export,
    &sh_help,
    &sh_pause,
    &sh_quit
};

// array of special characters
int symbols[] = {
    PIPE,
    LEFT,
    RIGHT
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

        // print the prompt
        printf("%s>", prompt);

        // read and parse line
        line = read_line();
        args = split_line(line);

        // update status
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

    // background flag
    int bg = FALSE;

    // process name to run
    char proc_name[strlen(args[0])];
    memset(proc_name, '\0', sizeof(proc_name));
    
    // check if there is '&' indicating background process
    if (args[0][strlen(args[0]) - 1] == '&') {
        strncpy(proc_name, args[0], strlen(args[0]) - 1);
        bg = TRUE;
    } else {
        // if no & just copy args[0] into proc_name
        strcpy(proc_name, args[0]);
    }

    pid = Fork();
    if (pid == 0) {
        // child process
        if (execvp(proc_name, args) < 0) {
            // error exec-ing
            fprintf(stderr, "shell: command not found: %s\n", args[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        // parent process
        // don't wait if it's a background process
        if (!bg) {
            do {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
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

    // detect the special character and its position
    int sym = detect_symbol(args);
    int sym_pos = detect_symbol_pos(args);

    // in the event there is no pipe or redirection
    if (sym < 0 || sym_pos < 0) {
        for (int i = 0; i < num_builtins(); i++) {
            if (strcmp(args[0], builtin_cmd[i]) == 0) {
                // this is a built in command
                return (*builtin_func[i])(args);
            }
        }
        return proc_launch(args);

    // there is pipe and redirection
    } else {
        // length of args array
        int len = args_len(args);
        // args to the left
        char *args_left[sym_pos + 1];
        // args to the right
        char *args_right[len - sym_pos];

        // copy arguments on the left side
        int i = 0;
        for (; i < sym_pos; i++)
            args_left[i] = args[i];
        args_left[i++] = NULL;

        // copy arguments on the right side
        int j = 0;
        for (; args[i] != NULL; i++, j++)
            args_right[j] = args[i];
        args_right[j] = NULL;

        // for (int i = 0; args_left[i] != NULL; i++)
        //     printf("DEBUG: args_left[%d]=%s\n", i, args_left[i]);
        // for (int i = 0; args_right[i] != NULL; i++)
        //     printf("DEBUG: args_right[%d]=%s\n", i, args_right[i]);
    }
}

// detect '|', '<<', '>>', '<', '>' in the command
int detect_symbol(char **args) {
    /**
    * '|' return 0
    * '<' return 1
    * '>' return 2
    *
    * if there is no symbol or empty command then return -1
    */

    // command is empty
    if (args[0] == NULL)
        return -1;

    for (int i = 0; args[i] != NULL; i++) {
        if (args[i][0] == PIPE)
            return 0;
        if (args[i][0] == LEFT)
            return 1;
        if (args[i][0] == RIGHT)
            return 2;
    }

    return -1;
}

// like the previous method, but instead return the position of the special character
int detect_symbol_pos(char **args) {
    // if the command is empty
    if (args[0] == NULL)
        return -1;

    for (int i = 0; args[i] != NULL; i++) {
        if (args[i][0] == PIPE ||
            args[i][0] == LEFT ||
            args[i][0] == RIGHT)
            return i;
    }

    // return -1 if nothing found
    return -1;
}
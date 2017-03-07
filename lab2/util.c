/*
 * =====================================================================================
 *
 *       Filename:  shell.c
 *
 *    Description:  Place to hold built-in function
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


extern char **environ;


// built in function for cd
int sh_cd(char **args) {
    // if there is no argument, change to $HOME
    if (args[1] == NULL) {
        // to be filled
        char *home;
        if ((home = getenv("HOME")) == NULL)
            fprintf(stderr, "cannot get $HOME");
        else {
            if (chdir(home) == -1)
                fprintf(stderr, "cd: %s\n", strerror(errno));
        }
    } else {
        if (chdir(args[1]) == -1)
            fprintf(stderr, "cd: %s: %s\n", strerror(errno), args[1]);
    }

    return 1;
}

// built in functinon for quit
int sh_quit(char **args) {
    // quit by just returning 0
    return 0;
}

// built in function for help (man more)
int sh_help(char **args) {
    char *man[] = {"man", "more", NULL};
    return proc_launch(man);
}

int sh_echo(char **args) {
    return 0;
}

int sh_environ(char **args) {
    for (int i = 0; environ[i] != NULL; i++)
        printf("%s\n", environ[i]);

    return 1;
}

// built in function for export
int sh_export(char **args) {
    // if there is no argument, then export acts the same as env
    if (args[1] == NULL) {
        char *env[] = {"env", NULL};
        return proc_launch(env);
    } else {
        if (putenv(args[1]) != 0) {
            fprintf(stderr, "cannot export environment");
        }
    }
    return 1;
}

pid_t Fork(void) {
    pid_t pid;

    if ((pid = fork()) < 0) {
        fprintf(stderr, "fork error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return pid;
}
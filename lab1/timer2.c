/*
 * =====================================================================================
 *
 *       Filename:  timer2.c
 *
 *    Description:  Timer 2 for application
 *
 *        Version:  1.0
 *        Created:  02/07/2017 22:09:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tu V.T. Tran (tuvttran), tuvtran97@gmail.com
 *        Company:  Temple University
 *
 * =====================================================================================
 */

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

pid_t Fork(void);

int main(int argc, char **argv) {
    pid_t child_pid;

    child_pid = Fork();

    if (child_pid == 0) {           // child process
        printf("Child. PID = %d.\n", getpid());

        if (execl("./application", "file1.txt", NULL) < 0) {
            fprintf(stderr, "exec error: %s.\n", strerror(errno));
        }
    } else {                        // parent process
        printf("Parent. PID = %d. Child PID = %d.\n", getpid(), child_pid);
    }

    return EXIT_SUCCESS; 
}

pid_t Fork(void) {
    pid_t pid;

    if ((pid = fork()) < 0) {
        fprintf(stderr, "fork error: %s.\n", strerror(errno));
        exit(0);
    }

    return pid;
}
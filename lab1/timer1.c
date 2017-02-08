/*
 * =====================================================================================
 *
 *       Filename:  timer1.c
 *
 *    Description:  Timer 1 for application
 *
 *        Version:  1.0
 *        Created:  02/07/2017 21:49:01
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
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

pid_t Fork(void);

int main(int argc, char **argv) {
    struct timeval begin, end;
    pid_t child_pid;
    int status;

    // timing stuff
    if (gettimeofday(&begin, NULL) < 0) {
        fprintf(stderr, "gettimeofday failed.\n");
        return EXIT_FAILURE;
    }

    child_pid = Fork();

    if (child_pid == 0) {           // child process
        printf("Child. PID = %d.\n", getpid());

        if (execl("./application", "./application", "file1.txt", NULL) < 0) {
            fprintf(stderr, "exec error: %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }

        exit(-1);
    } else {                        // parent process
        printf("Parent. PID = %d. Child PID = %d.\n", getpid(), child_pid);

        if (gettimeofday(&end, NULL) < 0) {
            fprintf(stderr, "gettimeofday failed.\n");
            return EXIT_FAILURE;
        }

        if (wait(&status) < 0) {
            fprintf(stderr, "wait error %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }

        float duration = (end.tv_usec - begin.tv_usec);
        printf("Runtime of child process %.1f microseconds.\n", duration);
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
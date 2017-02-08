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
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

pid_t Fork(void);

int main(int argc, char **argv) {
    struct timeval begin1, begin2, end;
    pid_t child_pid1, child_pid2;
    int status;

    // timing stuff
    if (gettimeofday(&begin1, NULL) < 0) {
        fprintf(stderr, "gettimeofday failed.\n");
        return EXIT_FAILURE;
    }

    child_pid1 = Fork();

    if (child_pid1 == 0) {           // child process
        printf("Child 1. PID = %d.\n", getpid());

        if (execl("./application", "./application", "file1.txt", NULL) < 0) {
            fprintf(stderr, "exec error: %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }

        exit(-1);
    } else {                        // parent process
        if (gettimeofday(&begin2, NULL) < 0) {
            fprintf(stderr, "gettimeofday failed.\n");
            return EXIT_FAILURE;
        }

        child_pid2 = Fork();

        if (child_pid2 == 0) {      // child process
            printf("Child 2. PID = %d.\n", getpid());

            if (execl("./application", "./application", "file2.txt", NULL) < 0) {
                fprintf(stderr, "exec error: %s.\n", strerror(errno));
                return EXIT_FAILURE;
            }

            exit(-1);
        } else {                   // parent process
            printf("Parent. PID = %d. Child 1 PID = %d. Child 2 PID = %d.\n", getpid(), 
                                                                        child_pid1, child_pid2);

            // timer end for application 1
            if (gettimeofday(&end, NULL) < 0) {
                fprintf(stderr, "gettimeofday failed.\n");
                return EXIT_FAILURE;
            }

            // reaping children
            if (waitpid(child_pid1, &status, 0) < 0) {
                fprintf(stderr, "wait error %s.\n", strerror(errno));
                return EXIT_FAILURE;
            }

            if (waitpid(child_pid2, &status, 0) < 0) {
                fprintf(stderr, "wait error %s.\n", strerror(errno));
                return EXIT_FAILURE;
            }

            float duration1 = (end.tv_usec - begin1.tv_usec);
            float duration2 = (end.tv_usec - begin2.tv_usec);
            printf("Time from starting up to launching %.1f microseconds.\n", duration1);
            printf("Time from starting up to launching %.1f microseconds.\n", duration2);
        }
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
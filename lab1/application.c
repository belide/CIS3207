/*
 * =====================================================================================
 *
 *       Filename:  application.c
 *
 *    Description:  Application to read and write file
 *
 *        Version:  1.0
 *        Created:  02/07/2017 12:27:53
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

#define BUF_SIZE 120

void write_to_file(int fd, char records[10][120]);     // write to file in each pass
void read_from_file(int fd, char records[10][120]);    // read from file in each pass

int main(int argc, char **argv) {
    int fd;         // file descriptor
    char *filename;
    char records[10][120];

    if (argc < 2) {
        fprintf(stderr, "Program %s. No file name entered. Terminated.\n", argv[0]);
        return EXIT_FAILURE;
    }

    filename = argv[1];

    if ((fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0755)) == -1) {
        fprintf(stderr, "error opening %s for reading and writing: %s\n", filename, strerror(errno));
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 100; i++) {
        write_to_file(fd, records);
        read_from_file(fd, records);
    }

    close(fd);
    if (unlink(filename) == -1) {
        fprintf(stderr, "error removing file: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void write_to_file(int fd, char records[10][120]) {
    // skip to the end of the file
    lseek(fd, 0, SEEK_END);

    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 120; i++) {
            records[j][i] = rand() % 58 + 65;
            if ((write(fd, &records[j][i], 1)) != 1) {
                fprintf(stderr, "error writing: %s\n", strerror(errno));
                return;
            }
        }
    }
}

void read_from_file(int fd, char records[10][120]) {
    // position to start reading from
    int p = rand() % 10;
    char c[120];

    // select a position from the file to read
    lseek(fd, -p*120, SEEK_END);

    if ((read(fd, &c, 120)) == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return;
    }

    for (int i = 0; i < 10; i++) {
        int matched = 1;
        for (int j = 0; j < 120; j++) {
            if (c[j] != records[i][j]) {
                matched = 0;
                break;
            } else
                continue;
        }

        if (matched)
            printf("MATCHED! %s\n", c);
    }
}

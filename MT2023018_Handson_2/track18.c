/*
Name : 18.c
Author : Vikram Virwani
Description :  Write a program to find out total number of directories on the pwd. execute ls -l | grep ^d | wc ? Use only dup2
Date: 20th Oct,2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int fd1[2];
    int fd2[2];
    int pid1, pid2;

    pipe(fd1);

    pid1 = fork();

    if (pid1 == 0) {
        close(fd1[0]);
        dup2(fd1[1], 1);
        close(fd1[1]);
        execlp("ls", "ls", "-l", NULL);
    } else {
        pipe(fd2);
        pid2 = fork();

        if (pid2 == 0) {
            close(fd1[1]);
            dup2(fd1[0], 0);
            close(fd1[0]);
            close(fd2[0]);
            dup2(fd2[1], 1);
            close(fd2[1]);
            execlp("grep", "grep", "^d", NULL);
            } 
	else {
            close(fd1[0]);
            close(fd1[1]);
            close(fd2[1]);
            dup2(fd2[0], 0);
            close(fd2[0]);
            execlp("wc", "wc", "-l", NULL);
        }
    }
}


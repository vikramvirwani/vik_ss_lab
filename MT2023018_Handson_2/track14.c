/*
Name : 14.c
Author : Vikram Virwani
Description : Write a simple program to create a pipe, write to the pipe, read from pipe and display on the monitor
Date: 20th Oct,2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void main() {
    int fd[2];
    pipe(fd);
    int pid = fork();

    if (pid == 0) {
        close(fd[1]);
        char buffer[100];
        int ns;

        while ((ns = read(fd[0], buffer, sizeof(buffer))) > 0) {
            write(1, buffer, ns);
        }

        close(fd[0]);
    }
    else {
        close(fd[0]);
        char msg[] = "Hey!This is parent\n";
        write(fd[1],msg,sizeof(msg));
	close(fd[1]);
    }
}


/*
Name : 15.c
Author : Vikram Virwani
Description : Write a simple program to send some data from parent to the child process
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

void main() {
    int fd[2];
    pipe(fd);
    int pid = fork();

    if (pid == 0) {
        close(fd[1]);
        char buffer[100];
        int ns;
	read(fd[0],buffer,sizeof(buffer));
       	printf("Message from parent %s\n",buffer);
        close(fd[0]);
    }
    else {
        close(fd[0]);
        char msg[] = "Hey!This is parent\n";
        write(fd[1],msg,sizeof(msg));
        close(fd[1]);
    }
}


/*
Name : 17b.c
Author : Vikram Virwani
Description : Write a program to execute ls -l | wc.
 a. use dup
 b. use dup2
 c. use fcntl
Date: 20th Oct,2023.
*/

#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

void main(){
        int fd[2];
        int pid;
        pipe(fd);
        pid=fork();
	if (pid==0) {
                close(fd[0]);
                close(1);
                fcntl(fd[1], F_DUPFD, 1);
                //dup2(fd[1], 1);
                //close(fd[1]);
                execlp("ls", "ls", "-l", NULL);
        }
        else{
                close(fd[1]);
                close(0);
                fcntl(fd[0], F_DUPFD, 0);
                //dup2(pipe_fd[0], STDIN_FILENO);
                //close(fd[0]);
                execlp("wc", "wc", NULL);
        }
}

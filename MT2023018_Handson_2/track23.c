/*
Name : 23.c
Author : Vikram Virwani
Description : Write a program to print the maximum number of files can be opened within a process and size of a pipe (circular buffer).
Date: 20th Oct,2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

void main()
{
	long fileCnt = sysconf(_SC_OPEN_MAX);
	 if (fileCnt == -1) {
        	perror("sysconf");
        	exit(EXIT_FAILURE);
   	 }
	 printf("Maximum number of files that can be opened: %ld\n", fileCnt);

    	int fd[2];
    	if (pipe(fd) == -1) {
       		 perror("pipe");
       		 exit(EXIT_FAILURE);
    	}
    	long pipeSize = fpathconf(fd[0], _PC_PIPE_BUF);
    	if (pipeSize == -1) {
        	perror("fpathconf");
        	exit(EXIT_FAILURE);
    	}

    	printf("Size of a pipe (circular buffer): %ld bytes\n", pipeSize);
}

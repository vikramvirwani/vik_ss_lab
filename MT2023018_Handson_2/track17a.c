/*
Name : 17a.c
Author : Vikram Virwani
Description : Write a program to execute ls -l | wc.
 a. use dup
 b. use dup2
 c. use fcntl
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>

void main(){
	int fd[2];
	int pid;
	pipe(fd);
	pid=fork();

	if(pid==0)
	{
		close(1);
		close(fd[0]);
		dup(fd[1]);
		execlp("ls","ls","-l",NULL);
	}	
	else{
		close(0);
		close(fd[1]);
		dup(fd[0]);
		execlp("wc","wc",NULL);
	}

}

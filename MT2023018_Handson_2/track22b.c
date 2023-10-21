/*
Name : 22b.c
Author : Vikram Virwani
Description : Write a program to wait for data to be written into FIFO within 10 seconds, use select system call with FIFO
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<sys/select.h>

void main(){
	char buf[100];
	mkfifo("myfifo",0777);
	int fd=open("myfifo",O_WRONLY);
	printf("Enter the text:");
	scanf(" %[^\n]",buf);
	write(fd,buf,sizeof(buf));
	close(fd);
}

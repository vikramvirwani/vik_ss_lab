/*
Name : 20b.c
Author : Vikram Virwani
Description : Write two programs so that both can communicate by FIFO -Use one way communication
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>

void main(){
	int fd=open("myfifoi",O_RDONLY);
	char msg[100];
	read(fd,msg,sizeof(msg));
	printf("%s\n",msg);
	close(fd);
}

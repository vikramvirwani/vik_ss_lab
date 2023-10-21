/*
Name : 22a.c
Author : Vikram Virwani
Description : Write a program to wait for data to be written into FIFO within 10 seconds, use select system call with FIFO
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<sys/select.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<fcntl.h>

int main()
{
	fd_set rfds;
	struct timeval tv;
	//mkfifo("myfifo",0666);
	int fd=open("myfifo",O_RDONLY);
	if(fd==-1)
	{
		printf("error in opening fifo");
		return -1;
	}
	int retval;
	tv.tv_sec=10;
	tv.tv_usec=0;
	FD_ZERO(&rfds);
	FD_SET(fd,&rfds);
	retval=select(fd+1,&rfds,NULL,NULL,&tv);
	if(retval==-1)
	{
		perror("Error in select\n");
	}
	else if(retval==0)
	{
		printf("datais not available in 10 seconds");
	}
	else
	{
		if(FD_ISSET(fd,&rfds)){
			printf("Data is available now\n");
			char buf[100];
			int br=read(fd,buf,sizeof(buf));
			buf[br]='\0';
			printf("Data from fifo : %s\n",buf);
		}
	}
	close(fd);
	return 0;
}

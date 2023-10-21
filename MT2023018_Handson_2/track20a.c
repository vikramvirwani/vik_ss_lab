/*
Name : 20a.c
Author : Vikram Virwani
Description : Write two programs so that both can communicate by FIFO -Use one way communication
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>

void main() {
    const char *fifo_path = "myfifoi";
    if(mkfifo(fifo_path, 0666)<0)
    {
	    perror("error in fifo\n");
	    return;
    }
    int fd = open(fifo_path, O_WRONLY, 0777);
    char message[] = "Hello! Another Program. \n";
    write(fd, message, sizeof(message));
    close(fd);
}

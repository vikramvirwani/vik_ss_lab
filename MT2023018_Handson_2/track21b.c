/*
Name : 21b.c
Author : Vikram Virwani
Description : Write two programs so that both can communicate by FIFO -Use two way communication
Date: 20th Oct,2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
void main()
{
    int fd1 = open("21afifo", O_RDONLY);
    char buffer1[100];
    int fd2=open("21bfifo",O_WRONLY);
    int br = read(fd1, buffer1, sizeof(buffer1));
    buffer1[br]='\0';
    printf("Received from FIFO1: %s\n",buffer1);
    char buffer2[100];
    printf("Enter the text\n");
    scanf(" %[^\n]",buffer2);
    write(fd2,buffer2,sizeof(buffer2));
    close(fd1);
    close(fd2);
}

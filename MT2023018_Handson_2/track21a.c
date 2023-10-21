/*
Name : 21a.c
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
    int fd1 = open("21afifo", O_WRONLY, 0777);
    int fd2=open("21bfifo",O_RDONLY,0777);
    char buffer1[100];
    printf("Enter some message : \n");
    scanf(" %[^\n]",buffer1);
    write(fd1, buffer1, sizeof(buffer1));
    char buffer2[100];
    int br = read(fd2, buffer2, sizeof(buffer2));
    buffer2[br]='\0';
    printf("The text from FIFO file: %s\n",buffer2);
    close(fd1);
    close(fd2);
}

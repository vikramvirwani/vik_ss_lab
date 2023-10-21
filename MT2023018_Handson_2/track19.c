/*
Name : 19.c
Author : Vikram Virwani
Description :  Create a FIFO file by
 a. mknod command
 b. mkfifo command
 c. use strace command to find out, which command (mknod or mkfifo) is better.
 c. mknod system call
 d. mkfifo library function
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

void main(){
	mknod("myfifo3",S_IFIFO|0666,0);
	mkfifo("myfifo4",0666);
}

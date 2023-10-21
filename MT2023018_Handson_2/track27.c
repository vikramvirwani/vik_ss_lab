/*
Name : 27.c
Author : Vikram Virwani
Description : Write a program to receive messages from the message queue.
 a. with 0 as a flag
 b. with IPC_NOWAIT as a flag
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void main(){
	struct msgbuf{
		long mtype;
		char mtext[100];
	}msg[2];
	key_t key=ftok(".",'a');
	int msgid=msgget(key,0666);
	ssize_t res=msgrcv(msgid,(void*)&msg[0],100,1,0);
	printf("key=> %ld & text=> %s\n",msg[0].mtype,msg[0].mtext);
	if(res==-1)
		return;	
	if(msgrcv(msgid,(void*)&msg[1],100,2,IPC_NOWAIT)==-1)
	{
		printf("Error\n");
		return;
	}
	else
		printf("key=> %ld & text=> %s\n",msg[1].mtype,msg[1].mtext);
}

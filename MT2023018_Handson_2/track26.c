/*
Name : 26.c
Author : Vikram Virwani
Description : Write a program to send messages to the message queue. Check $ipcs -q
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

struct msgbuf {
           long mtype;
           char mtext[80];
       };


void main()
{
	struct msgbuf{
		long mtype;
		char mtext[100];
	}msg[2];
	//struct msgbuf msg;
	int key=ftok(".",'a');
	int msgid=msgget(key,0777|IPC_CREAT);
	for(int i=0;i<2;i++)
	{
		printf("Enter the message key :\n");
		scanf("%ld",&msg[i].mtype);
		printf("Enter the message :\n");
		scanf(" %[^\n]",msg[i].mtext);
		getchar();
	//	getchar();
		if(msgsnd(msgid,&msg[i],sizeof(msg[i].mtext)+1,0)==-1)
		{
			perror("msgsnd");
			return;
		}
		printf("mtext => %s\n",msg[i].mtext);
	}
	system("ipcs -q");
}

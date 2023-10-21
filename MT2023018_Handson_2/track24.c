/*
Name : 24.c
Author : Vikram Virwani
Description : Write a program to create a message queue and print the key and message queue id
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

void main()
{
	int k=ftok("/tmp",'B');
	if(k==-1)
	{
		perror("error");
		return;
	}
	int msgid=msgget(k,0777|IPC_CREAT);
	if(msgid==-1)
	{
		perror("error int msgget");
		return;
	}
	printf("Key => %d\n",k);
	printf("Message id => %d\n",msgid);
}

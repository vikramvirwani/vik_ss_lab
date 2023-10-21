/*
Name : 29.c
Author : Vikram Virwani
Description : Write a program to remove the message queue
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

void main(){
	int key=ftok("/tmp",'A');
	int msgid=msgget(key,0666);
	msgctl(msgid,IPC_RMID,NULL);
	printf("Message queue removed\n");
}

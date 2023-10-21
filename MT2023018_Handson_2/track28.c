/*
Name : 28.c
Author : Vikram Virwani
Description : Write a program to change the exiting message queue permission. (use msqid_ds structure)
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<time.h>

void main(){
	struct msqid_ds st;
	int key =ftok("/tmp",'A');
	int msgid=msgget(key,0777|IPC_CREAT);
	msgctl(msgid,IPC_STAT,&st);
	printf("%o\n",st.msg_perm.mode);
	st.msg_perm.mode=0666;
	msgctl(msgid,IPC_SET,&st);
	printf("%o\n",st.msg_perm.mode);
}

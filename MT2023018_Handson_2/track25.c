/*
Name : 25.c
Author : Vikram Virwani
Description : Write a program to print a message queue's (use msqid_ds and ipc_perm structures)
 a. access permission
 b. uid, gid
 c. time of last message sent and received 
 d. time of last change in the message queue
 d. size of the queue
 f. number of messages in the queue 
 g. maximum number of bytes allowed 
 h. pid of the msgsnd and msgrcv
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<stdlib.h>
#include<time.h>

void main(){
	int key=ftok("/tmp",'A');
	int msgid=msgget(key,0777|IPC_CREAT);
	struct msqid_ds st;
	int msi = msgctl(msgid, IPC_STAT, &st);
	printf("%o\n",st.msg_perm.mode);
	printf("%d\n",st.msg_perm.uid);
	printf("%d\n",st.msg_perm.gid);
	printf("%s\n",ctime(&st.msg_stime));
	printf("%s\n",ctime(&st.msg_rtime));
	printf("%s\n",ctime(&st.msg_ctime));
	printf("%ld\n",st.msg_qbytes);
	printf("%ld\n",st.msg_qnum);
	printf("%ld\n",st.msg_cbytes);
	printf("Send id %d\n",st.msg_lspid);
	printf("Recieve id %d\n",st.msg_lrpid);
}

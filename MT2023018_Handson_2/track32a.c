/*
Name : 32a.c
Author : Vikram Virwani
Description : Write a program to implement semaphore to protect any critical section.
 a. rewrite the ticket number creation program using semaphore
 b. protect shared memory from concurrent write access
 c. protect multiple pseudo resources ( may be two) using counting semaphore
 d. remove the created semaphore
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/sem.h>
#include<sys/ipc.h>

void main(){
	int fd=open("ticket.txt",O_CREAT|O_RDWR,0777);
	union semun{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
		struct seminfo *_buf;
	}semSet;
	int semKey=ftok(".",331);
	int semIdentifier=semget(semKey,1,0);
	int semctlStatus;
	if(semIdentifier==-1)
	{
		semIdentifier=semget(semKey,1,IPC_CREAT|0700);
		semSet.val=1;
		semctlStatus=semctl(semIdentifier,0,SETVAL,semSet);
	}
	int data;
	struct sembuf semOp;
	semOp.sem_num=0;
	semOp.sem_flg=0;
	printf("Press Enter to obtain lock on CS\n");
	getchar();
	semOp.sem_op=-1;
	int semopStatus=semop(semIdentifier,&semOp,1);
	printf("Obtained lock on CS\n");
	printf("Now Entering CS\n");

	int rb=read(fd,&data,sizeof(data));
	if(rb<0)
	{
		perror("Error in reading the ticket\n");
		return;
	}
	else if(rb==0)
		data=1;
	else{
		data+=1;
		lseek(fd,0,SEEK_SET);
	}
	int wb=write(fd,&data,sizeof(data));
	printf("Your ticket no. is %d\n",data);
	printf("Press enter to exit\n");
	getchar();

	semOp.sem_op=1;
	semopStatus=semop(semIdentifier,&semOp,1);
	close(fd);
}

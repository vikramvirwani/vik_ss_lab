/*
Name : 32c.c
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
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<fcntl.h>

void main(){
	int fd=open("xyz.txt",O_CREAT|O_RDONLY,0777);
	union semun
	{
		int val;
	}semSet;
	int semKey=ftok(".",331);
	int semIdentifier=semget(semKey,1,0);
	int semctlStatus;
	if(semIdentifier<0)
	{
		semIdentifier=semget(semKey,1,IPC_CREAT|0700);
		semSet.val=2;
		semctlStatus=semctl(semIdentifier,0,SETVAL,semSet);
	}
	struct sembuf semOp;
	semOp.sem_num=0;
	semOp.sem_flg=IPC_NOWAIT;
	printf("Press enter in the critical section\n");
	getchar();
	semOp.sem_op=-1;
	int semopStatus=semop(semIdentifier,&semOp,1);
	printf("Obtained CS\n");
	printf("Entering CS\n");
	char data[1000];
	int rb=read(fd,&data,sizeof(data));
	if(rb<0)
	{
		perror("Error in reading sample file\n");
		return;
	}
	else if(rb==0)
	{
		printf("No data exists\n");
	}
	else{
		printf("Sample data:\n%s",data);
	}
	printf("Press Enter to exit CS\n");
	getchar();

	semOp.sem_op=1;
	semopStatus=semop(semIdentifier,&semOp,1);

	close(fd);
}

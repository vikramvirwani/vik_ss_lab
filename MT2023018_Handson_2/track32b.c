/*
Name : 32b.c
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

void main()
{
	union semun{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
		struct seminfo *_buf;
	}semSet;
	int semKey=ftok(".",331);
	int semIdentifier=semget(semKey,1,0);
	int semctlStatus;
	if(semIdentifier<0)
	{
		semIdentifier=semget(semKey,1,IPC_CREAT|0700);
		semSet.val=1;
		semctlStatus=semctl(semIdentifier,0,SETVAL,semSet);
	}

	int shmKey=ftok(".",3322);
	int shmSize=20;
	int shmIdentifier=shmget(shmKey,shmSize,IPC_CREAT|0700);
	char *shmPointer;
	shmPointer=shmat(shmIdentifier,(void *)0,0);
	struct sembuf semOp;
	semOp.sem_num=0;
	semOp.sem_flg=0;
	printf("Press Enter to lock the CS\n");
	getchar();

	semOp.sem_op=-1;
	int semopStatus=semop(semIdentifier,&semOp,1);
	printf("Inside the Critical Section\n");
	printf("Writing in the CS\n");
	sprintf(shmPointer,"Hello");
	printf("Press enter to read from the shared memory\n");
	getchar();
	printf("%s\n",shmPointer);
	printf("Press Enter to exit CS\n");
	getchar();

	semOp.sem_op=1;
	semopStatus=semop(semIdentifier,&semOp,1);
	printf("CS is unlocked\n");
}

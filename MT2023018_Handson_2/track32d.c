/*
Name : 32d.c
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
	printf("Press Enter to Create Semaphore\n");
	getchar();
	int semKey=ftok(".",1);
	int semIdentifier=semget(semKey,1,IPC_CREAT|0777);
	if(semIdentifier<0)
	{
		perror("error in creating semaphore\n");
		return;
	}
	printf("Semaphore Created\n");
	printf("Press Enter to delete the semaphore");
	getchar();
	int status=semctl(semIdentifier,0,IPC_RMID);
	if(status<0)
	{
		perror("Error in deleting semaphore\n");
		return;
	}
}

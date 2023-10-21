/*
Name : 30.c
Author : Vikram Virwani
Description : Write a program to create a shared memory.
 a. write some data to the shared memory
 b. attach with O_RDONLY and check whether you are able to overwrite.
 c. detach the shared memory
 d. remove the shared memory
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void main(){
	int key=ftok(".",'A');
	int shmid=shmget(key,1024,IPC_CREAT|0777);
	char *data;
	data=shmat(shmid,0,0);
	printf("Enter the text:\n");
	scanf("%[^\n]",data);
	getchar();
	system("ipcs -m");
	char *otpt;
	otpt=shmat(shmid,0,SHM_RDONLY);
	//int x=(int *)(int)otpt;
	if(otpt<(char *)-1)
	{
		printf("read only shared memory\n");
	}
	else
	{
		printf("shared memory is ok\n");
	}
	system("ipcs -m");
	if(shmdt(otpt)==-1)
	{
		perror("error\n");
		return;
	}
	struct shmid_ds st;
	int rm=shmctl(shmid,IPC_RMID,&st);
	if(rm!=-1)
		printf("deleted\n");
	else
		printf("not deleted\n");
}

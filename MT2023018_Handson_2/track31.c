/*
Name : 31.c
Author : Vikram Virwani
Description : Write a program to create a semaphore and initialize value to the semaphore.
 a. create a binary semaphore
 b. create a counting semaphore
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>

void main(){
	sem_t b_sem;
	int x=sem_init(&b_sem,0,1);
	if(x==0)
	{
		printf("Binary Semaphore Created\n");
		sem_destroy(&b_sem);
	}
	else{
		printf("Error int Binary Semaphore\n");
		return;
	}
	sem_t c_sem;
	int y=sem_init(&c_sem,0,5);
	if(y==0)
	{
		printf("Counting Semaphore Created\n");
	}
	else{
		printf("Error in Counting Semaphore\n");
		return;
	}
}

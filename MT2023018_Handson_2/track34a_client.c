/*
Name : 34a_client.c
Author : Vikram Virwani
Description : Write a program to create a concurrent server.
 a. use fork
 b. use pthread_create
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>

void main(){
	int cli_sock;
	struct sockaddr_in ser_addr;
	cli_sock=socket(AF_INET,SOCK_STREAM,0);
	if(cli_sock<0)
	{
		printf("error in socket creation\n");
		return;
	}
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_port=htons(9326);
	ser_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

	int check=connect(cli_sock,(struct sockaddr*)&ser_addr,sizeof(ser_addr));
	if(check<0)
	{
		perror("Connection\n");
		return;
	}
	char msg[100];
	if(recv(cli_sock,msg,sizeof(msg),0)<0)
	{
		perror("error in receiving\n");
		return;
	}
	printf("%s\n",msg);
	//memset(msg,'\0',sizeof(msg));
	char msg_ser[1000];
	printf("Enter some Message for server\n");
	scanf("%s",msg_ser);
	if(send(cli_sock,msg_ser,sizeof(msg_ser),0)<0)
	{
		perror("error in sending message\n");
		return;
	}
	close(cli_sock);
}

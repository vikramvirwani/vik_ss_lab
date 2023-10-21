/*
Name : 33b.c
Author : Vikram Virwani
Description : Write a program to communicate between two machines using socket.
Date: 20th Oct,2023.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

void main(){
	int cli_sock;
	struct sockaddr_in serv_addr;

	cli_sock=socket(AF_INET, SOCK_STREAM, 0);
	if(cli_sock==-1)
	{
		perror("Error in socket creation\n");
		return;
	}

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(8998);
	serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

	int con=connect(cli_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	if(con==-1)
	{
		perror("Error connecting to server\n");
		return;
	}

	printf("Connected \n");
	char msg[100];
	printf("Write some Message to server\n");
	scanf("%s",msg);
	char cli_msg[100];
	memset(cli_msg,'\0',sizeof(cli_msg));
	strcpy(cli_msg,msg);
	int sent=send(cli_sock,cli_msg,sizeof(cli_msg),0);
	if(sent<0)
	{
		perror("error in sending msg\n");
		return;
	}
	char server_msg[100];
	int rec=recv(cli_sock,server_msg,sizeof(server_msg),0);
	if(rec<0)
	{
		perror("error in recv msg\n");
		return;
	}
	printf("Message received => \n %s\n",server_msg);
	close(cli_sock);
}

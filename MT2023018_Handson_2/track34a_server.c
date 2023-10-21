/*
Name : 34a_server.c
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

void main()
{
	int ser_sock,cli_sock;
	struct sockaddr_in serv_addr,cli_addr;
	ser_sock=socket(AF_INET,SOCK_STREAM,0);
	if(ser_sock<0)
	{
		perror("error in establishing connection\n");
		return;
	}
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	serv_addr.sin_port=htons(9326);

	int bd=bind(ser_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	if(bd<0)
	{
		perror("error in binding\n");
		return;
	}
	if(listen(ser_sock,3)<0)
	{
		perror("error in listen\n");
		return;
	}
	printf("Listening\n");
	while(1)
	{
		int addrlen=sizeof(cli_addr);
		cli_sock=accept(ser_sock,(struct sockaddr*)&cli_addr,&addrlen);
		if(cli_sock<0)
		{
			perror("error in connecting\n");
			return;
		}
		else{
			if(fork()==0)
			{
				//child
				char msg[100];
				//printf("Enter some message for the client =>\n");
				//scanf("%s",msg);
				strcpy(msg,"Hi from Client\n");
				int sent=send(cli_sock,msg,sizeof(msg),0);
				if(sent<0)
				{
					perror("error in sending message to client\n");
					return;
				}
				printf("Data sent to client\n");
				memset(msg,'\0',sizeof(msg));
				char ser_msg[1000];
				int rec=recv(cli_sock,ser_msg,sizeof(ser_msg),0);
				if(rec<0)
				{
					perror("Error in receiving message from client\n");
					return;
				}
				printf("Received message from client => %s\n",ser_msg);
			}
			else{
				close(cli_sock);
			}
		}
	}
	close(ser_sock);
}

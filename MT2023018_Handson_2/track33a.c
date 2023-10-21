/*
Name : 33a.c
Author : Vikram Virwani
Description : Write a program to communicate between two machines using socket.
Date: 20th Oct,2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void main(){
	int ser_sock,cli_sock;
	struct sockaddr_in serv_addr,cli_addr;

	ser_sock=socket(AF_INET,SOCK_STREAM,0);
	if(ser_sock<0)
	{
		perror("Error in creating server socket\n");
		return;
	}

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(8998);
	serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	socklen_t client_address_length = sizeof(cli_addr);
	int bd=bind(ser_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	if(bd<0)
	{
		perror("Error in binding\n");
		return;
	}
	if(listen(ser_sock,4)<0)
	{
		perror("listen\n");
		return;
	}
	printf("Server is listening on port 8998...\n");
	int addrlen=sizeof(cli_addr);
	cli_sock=accept(ser_sock,(struct sockaddr*)&cli_addr,&addrlen);
	if(cli_sock<0)
	{
		perror("Error in accepting connection\n");
		return;
	}
	printf("Connection accepted\n");
	char msg[100];
	/*
	printf("Enter the message\n");
	scanf("%s",msg);
	*/
	memset(msg,'\0',sizeof(msg));
	int rec=recv(cli_sock,msg,sizeof(msg),0);
	if(rec<0)
	{
		perror("error sending\n");
		return;
	}
	printf("Message from client => %s\n",msg);
	memset(msg,'\0',sizeof(msg));
	printf("Enter some message for client =>\n");
	scanf("%s",msg);
	if(send(cli_sock,msg,sizeof(msg),0)<0)
	{
		perror("error in sending\n");
		return;
	}
	//printf("%s\n",msg);
	close(cli_sock);
	close(ser_sock);
}

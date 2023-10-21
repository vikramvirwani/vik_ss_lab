/*
Name : 34b_server.c
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
#include<pthread.h>

void communicate(int *fd)
{
	char msg[100];
	strcpy(msg,"Hi from Server\n");
	if(send(*fd,msg,sizeof(msg),0)<0)
	{
		perror("error in sending\n");
		return;
	}
	char cli_msg[1000];
	int c=recv(*fd,cli_msg,sizeof(cli_msg),0);
	if(c<0)
	{
		perror("Error in receiving message from client\n");
		return;
	}
	printf("Message from client => %s",cli_msg);
	close(*fd);
}

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
        serv_addr.sin_port=htons(4944);

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
	pthread_t threadId;
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
			if(pthread_create(&threadId,NULL,(void *)communicate,&cli_sock))
				perror("Error while creating thread\n");
		}
	}
	close(ser_sock);
}

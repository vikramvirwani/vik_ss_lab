/*
Name : 16.c
Author : Vikram Virwani
Description : Write a program to send and receive data from parent to child vice versa. Use two way communication
Date: 20th Oct,2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int pcfd[2];
    int cpfd[2];

    pipe(pcfd);
    pipe(cpfd);
    int pid = fork();

    if (pid == 0) {
        close(pcfd[1]);
        close(cpfd[0]);
        char message_from_parent[100];
        char message_to_parent[] = "Hello, parent!";
        read(pcfd[0], message_from_parent, sizeof(message_from_parent));
        printf("Child received from parent: %s\n", message_from_parent);
        write(cpfd[1], message_to_parent, sizeof(message_to_parent));
        close(pcfd[0]);
        close(cpfd[1]);
    } else {
        close(pcfd[0]);
        close(cpfd[1]);
        char message_to_child[] = "Hello, child!";
        char message_from_child[100];
        write(pcfd[1], message_to_child, sizeof(message_to_child));
        read(cpfd[0], message_from_child, sizeof(message_from_child));
        printf("Parent received from child: %s\n", message_from_child);
        close(pcfd[1]);
        close(cpfd[0]);
    }
}


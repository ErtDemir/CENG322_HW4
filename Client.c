// Created by Ertuğrul Demir on 4/20/2022.
// Ertuğrul Demir 260201059
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SERVERPORT 8887
#define MAXBUF 1024

int main(int argc, char* argv[])
{
    int sockd;
    int counter;
    int fd;
    struct sockaddr_in xferServer;
    char buf[MAXBUF];
    int returnStatus;
    int SIZE = 20;
    const char *specialChar[] = {"..","#", "%", "&", "{", "}", "<",">","*","?",";","$","!","'","\"",":","@","+","|","="};

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <ip address> <filename> [dest filename]\n",
                argv[0]);
        exit(1);
    }
// Control part


    if(strlen(argv[2]) >= 20){
        fprintf(stderr,"Filename must not too long !\n");
        exit(1);
    }

    for (int i = 0; i < SIZE; ++i) {
        if(strstr(argv[2], specialChar[i])){
            fprintf(stderr,"Cannot use special character in filename or directory !\n");
            exit(1);
        }
    }

    if( access( argv[2], F_OK ) != 0 ) {
        // file doesn't exist
        fprintf(stderr,"File does not exist !\n");
        exit(1);
    }



/* create a socket */
    sockd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockd == -1)
    {
        fprintf(stderr, "Could not create socket!\n");
        exit(1);
    }
/* set up the server information */
    xferServer.sin_family = AF_INET;
    xferServer.sin_addr.s_addr = inet_addr(argv[1]);
    xferServer.sin_port = htons(SERVERPORT);
/* connect to the server */
    returnStatus = connect(sockd,
                           (struct sockaddr*)&xferServer,
                           sizeof(xferServer));
    if (returnStatus == -1)
    {
        fprintf(stderr, "Could not connect to server!\n");
        exit(1);
    }

    /* send the name of the file we want to the server */
    returnStatus = write(sockd, argv[2], strlen(argv[2])+1);
    if (returnStatus == -1)
    {
        fprintf(stderr, "Could not send filename to server!\n");
        exit(1);
    }

    /* call shutdown to set our socket to read-only */
    shutdown(sockd, SHUT_WR);


    /* open up a handle to our destination file to receive the contents */
/* from the server */
    fd = open(argv[3], O_WRONLY | O_CREAT | O_APPEND);
    if (fd == -1)
    {
        fprintf(stderr, "Could not open destination file, using stdout.\n");
        fd = 1;
    }
    /* read the file from the socket as long as there is data */
    while ((counter = read(sockd, buf, MAXBUF)) > 0)
    {
/* send the contents to stdout */
        write(fd, buf, counter);
    }
    if (counter == -1)
    {
        fprintf(stderr, "Could not read file from socket!\n");
        exit(1);
    }
    close(sockd);
    return 0;
}
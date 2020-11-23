#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc , char *argv[])
{

    //socket的建立
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM, 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }
    printf("creat socket successful!\n");
    //socket的連線

    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;

    //localhost test
    info.sin_addr.s_addr = INADDR_ANY;
    info.sin_port = htons(20000);


    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error");
    }
    printf("connect successful\n");

    //Send a message to server
    char message[] = {"Hi there"};
    char receiveMessage[100] = {};
    while(1){
        send(sockfd,message,sizeof(message),0);
        recv(sockfd,receiveMessage,sizeof(receiveMessage),0);

        printf("%s",receiveMessage);
    }
    printf("close Socket\n");
    close(sockfd);
    return 0;
}
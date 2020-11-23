#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>


#define PORT 8888
#define MAX_LINE 2048
#define LISTENQ 20


int main(int argc , char **argv)
{
	int i, maxi, maxfd, listenfd, connfd, sockfd;

	int nready , client[FD_SETSIZE];
	
	ssize_t n, ret;
		
	fd_set rset , allset;

	char buf[MAX_LINE];

	socklen_t clilen;

	struct sockaddr_in servaddr , cliaddr;

	/*(1) 得到監聽描述符*/
	listenfd = socket(AF_INET , SOCK_STREAM , 0);

	/*(2) 繫結套接字*/
	bzero(&servaddr , sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	bind(listenfd , (struct sockaddr *)&servaddr , sizeof(servaddr));

	/*(3) 監聽*/
	listen(listenfd , LISTENQ);

	/*(4) 設定select*/
	maxfd = listenfd;
	maxi = -1;
	for(i=0 ; i<FD_SETSIZE ; ++i)
	{
		client[i] = -1;
	}//for
	FD_ZERO(&allset);
	FD_SET(listenfd , &allset);

    int flag[100];
    for(int i=0;i<100;i++){
        flag[i] = 0;
    }

	int people[100],play = 0,turn; 
	/*(5) 進入伺服器接收請求死迴圈*/
	while(1)
	{
		rset = allset;
		nready = select(maxfd+1 , &rset , NULL , NULL , NULL);
		
		if(FD_ISSET(listenfd , &rset))
		{
			/*接收客戶端的請求*/
			clilen = sizeof(cliaddr);

			printf("\naccpet connection~\n");

			if((connfd = accept(listenfd , (struct sockaddr *)&cliaddr , &clilen)) < 0)
			{
				perror("accept error.\n");
				exit(1);
			}//if		

			printf("accpet a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr) , cliaddr.sin_port);

			/*將客戶連結套接字描述符新增到陣列*/
			for(i=0 ; i<FD_SETSIZE ; ++i)
			{
				if(client[i] < 0)
				{
					client[i] = connfd;
					break;
				}//if
			}//for

			if(FD_SETSIZE == i)
			{
				perror("too many connection.\n");
				exit(1);
			}//if

			FD_SET(connfd , &allset);
			if(connfd > maxfd)
				maxfd = connfd;
			if(i > maxi)
				maxi = i;

			if(--nready < 0)
				continue;
		}//if

		for(i=0; i<=maxi ; ++i)
		{
			if((sockfd = client[i]) < 0)
				continue;
			if(FD_ISSET(sockfd , &rset))
			{
				/*處理客戶請求*/
				printf("\nreading the socket~~~ \n");
				bzero(buf , MAX_LINE);
				read(sockfd , buf , MAX_LINE);
                char line[1024];
                if(flag[sockfd]==0){ // not accept
						int byte;
				        printf("clint[%d] send message: %s\n", i , buf);
                        if(strcmp(buf,"bkbk\n")==0){
                            char *acc = "acceptd now\n";
                            write(sockfd,acc,strlen(acc));
                            flag[sockfd] = 1;
                        }else{
                            write(sockfd,"not accept please try again\n",strlen("not accept please try again\n"));
                            //fgets(line,1024,stdin);
                            //write(sockfd,line,strlen(line));
                        }
                }else if(flag[sockfd] == 1){ //accept now
					if(strcmp(buf,"l\n")==0){//列出人
						char people[100]  = {};
						for(int i=0 ; i<=maxi ; ++i){
							if(flag[client[i]]==1&&client[i]!=sockfd){
								char str[10];
								sprintf(str,"%d",client[i]);
								strcat(people,str);
								strcat(people,"    ");
							}
						}
						strcat(people,"\n");
						write(sockfd,people,strlen(people));
					}else if(strcmp(buf,"ok\n")==0){ ; }
					else if(strncmp(buf,"I win",strlen("I win"))==0){
						write(people[sockfd],"you lose and game over\n",strlen("you lose and game over\n"));
						write(sockfd,"game over\n",strlen("game over\n"));
						play = 0;
					}else if(strncmp(buf,"U win",strlen("U win"))==0){
						write(people[sockfd],"U win and game over\n",strlen("U win and game over"));
						write(sockfd,"game over\n",strlen("game over\n"));
						play = 0;
					}else if(strncmp(buf,"peace",strlen("peace"))==0){
						write(people[sockfd],"peace\n",strlen("peace\n"));
						write(sockfd,"game over\n",strlen("game over\n"));
						play = 0;
					}
					else if(play==1&&turn == sockfd){
						char *ptr,choice;
						write(people[sockfd],buf,strlen(buf));
						turn = people[sockfd];
					}else if(play==1 && turn != sockfd){
						write(people[sockfd],"invalid\n",strlen("invalid\n"));
					//	turn = people[sockfd];
					}
					else if(strncmp(buf,"s ",2)==0){ // 選擇對手
						int rival; char s;
						sscanf(buf,"%c %d",&s,&rival); //選擇的對手
		//				write(sockfd,"s\n",strlen("s\n"));
						write(rival,"you are chosen please input y or n to response",strlen("you are chosen please input y or n to response"));
						printf("rival = %d\n",rival);
						people[sockfd] = rival; people[rival] = sockfd; //紀錄對手是誰
					}else if(strcmp("look\n",buf)==0){ // 查看狀況
						write(sockfd,"\n",strlen("\n")); 
					}else if(strcmp(buf,"y\n")==0){ //同意
						write(people[sockfd],"the rival agree\n",strlen("the rival agree\n"));
					}else if(strcmp(buf,"n\n")==0){ //否認
						write(people[sockfd],"the rival refuse\n",strlen("the rival refuse\n"));
						write(sockfd,"ok\n",strlen("ok\n"));
					}else if(strncmp(buf,"O",1)==0){
						play = 1;
						turn = sockfd;
						write(sockfd,"start! u first\n",strlen("start! u first\n"));
						write(people[sockfd],"you are X and input ok\n",strlen("you are X and input ok\n"));
					}else if(strncmp(buf,"X",1)==0){
						play = 1;
						turn = people[sockfd];
						write(sockfd,"start! and input ok\n",strlen("start! and input ok\n"));
						write(people[sockfd],"you are O u first\n",strlen("you are O u first\n"));
					}else if(strncmp(buf,"log out",strlen("log out"))==0){
						write(sockfd,"log out successful\n",strlen("log out successful\n"));
						flag[sockfd] = 0;
					}
					else{ //查看指令
						write(sockfd,"please input the instructions below :		l : list people		s : select person look : check whethere to be selected\n",strlen("please input the instructions below :		l : list people		s : select person look : check whethere to be selected\n"));
						//continue;
					}
                }
//				printf("test~~~~~~~~~~~~~~~~~~~~~\n");
				if(--nready <= 0)
					break;
			}//if isset
		}//for
    //    flag[sockfd] = 0;
	}//while
}
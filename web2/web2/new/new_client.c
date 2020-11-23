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
#include <ctype.h>

#define PORT 8888
#define MAX_LINE 2048

//------------------------------------------------ tic tac toe--------------------------------------
char square[10] = { 'o', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

int checkwin()
{
    if (square[1] == square[2] && square[2] == square[3])
        return 1;
        
    else if (square[4] == square[5] && square[5] == square[6])
        return 1;
        
    else if (square[7] == square[8] && square[8] == square[9])
        return 1;
        
    else if (square[1] == square[4] && square[4] == square[7])
        return 1;
        
    else if (square[2] == square[5] && square[5] == square[8])
        return 1;
        
    else if (square[3] == square[6] && square[6] == square[9])
        return 1;
        
    else if (square[1] == square[5] && square[5] == square[9])
        return 1;
        
    else if (square[3] == square[5] && square[5] == square[7])
        return 1;
        
    else if (square[1] != '1' && square[2] != '2' && square[3] != '3' &&
        square[4] != '4' && square[5] != '5' && square[6] != '6' && square[7] 
        != '7' && square[8] != '8' && square[9] != '9')

        return 0;
    else
        return  - 1;
}

void board()
{
    system("cls");
    printf("\n\n\tTic Tac Toe\n\n");

    printf("     |     |     \n");
    printf("  %c  |  %c  |  %c \n", square[1], square[2], square[3]);

    printf("_____|_____|_____\n");
    printf("     |     |     \n");

    printf("  %c  |  %c  |  %c \n", square[4], square[5], square[6]);

    printf("_____|_____|_____\n");
    printf("     |     |     \n");

    printf("  %c  |  %c  |  %c \n", square[7], square[8], square[9]);

    printf("     |     |     \n\n");
}

int tic_tac_toe(int mark,char choice)
{
	int i;
	if(mark == 1){ mark = 'O'; }
	else if(mark == 2) { mark = 'X' ; }


    if (choice == '1' && square[1] == '1')
        square[1] = mark;
            
    else if (choice == '2' && square[2] == '2')
        square[2] = mark;
            
    else if (choice == '3' && square[3] == '3')
        square[3] = mark;
            
    else if (choice == '4' && square[4] == '4')
        square[4] = mark;
        
    else if (choice == '5' && square[5] == '5')
        square[5] = mark;
            
    else if (choice == '6' && square[6] == '6')
        square[6] = mark;
            
    else if (choice == '7'&& square[7] == '7')
        square[7] = mark;
            
    else if (choice == '8'&& square[8] == '8')
        square[8] = mark;
            
    else if (choice == '9'&& square[9] == '9')
        square[9] = mark;
            
    else
    {
        printf("Invalid move ");
          //  getch();
    }
    i = checkwin();
    
    board();
    
    return i;//要修改
    
}


int max(int a , int b)
{
	return a > b ? a : b;
}

/*readline函式實現*/
ssize_t readline(int fd, char *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = read(fd, &c,1)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}

int play = 0;
int player[2]; //1 O : 2 X
int judge=-2;
/*普通客戶端訊息處理函式*/
void str_cli(int sockfd)
{
	/*傳送和接收緩衝區*/
	char sendline[MAX_LINE] , recvline[MAX_LINE];

	while(fgets(sendline , MAX_LINE , stdin) != NULL)	
	{
		char *ptr,choice;
		ptr = sendline;
		if(isdigit(*ptr)&&play==1){
			choice = *ptr;
			judge = tic_tac_toe(player[0],choice);
		}

		if(judge == 1){
			printf("you win\n");
			write(sockfd,"I win\n",strlen("I win\n"));
			play = 0; judge = -2;
			for(int i = 1 ; i<=9 ; i++){//初始化
				char x;
				x = i + 48;
				square[i] = x;
			}
		}else if(judge  == 0){
			write(sockfd,"peace\n",strlen("peace\n"));
			play = 0; judge = -2;
			for(int i = 1 ; i<=9 ; i++){//初始化
				char x;
				x = i + 48;
				square[i] = x;
			}
		}else{
			write(sockfd , sendline , strlen(sendline));
		}

		bzero(recvline , MAX_LINE);
		if(readline(sockfd , recvline , MAX_LINE) == 0)
		{
			perror("server terminated prematurely");
			exit(1);
		}//if
		if(fputs(recvline , stdout) == EOF)
		{
			perror("fputs error");
			exit(1);
		}//if
		if(strncmp(recvline,"you lose and game over\n",strlen("you lose and game over\n"))==0){
			play = 0;
			for(int i = 1 ; i<=9 ; i++){//初始化
				char x;
				x = i + 48;
				square[i] = x;
			}
			continue;
		}else if(strncmp(recvline,"U win and game over\n",strlen("U win and game over\n"))==0){
			play = 0;
			for(int i = 1 ; i<=9 ; i++){//初始化
				char x;
				x = i + 48;
				square[i] = x;
			}
			continue;
		}else if(strncmp(recvline,"peace",strlen("peace"))==0){
			play = 0;
			for(int i = 1 ; i<=9 ; i++){//初始化
				char x;
				x = i + 48;
				square[i] = x;
			}
			continue;
		}
		ptr = recvline;
		if(play == 1 && isdigit(*ptr)){
			choice = *ptr;
			judge = tic_tac_toe(player[1],choice);
		}
		if(judge == 1){
			play = 0; judge = -2;
			for(int i = 1 ; i<=9 ; i++){//初始化
				char x;
				x = i + 48;
				square[i] = x;
			}
		}else if(judge == 0){
			play = 0; judge = -2;
			for(int i = 1 ; i<=9 ; i++){//初始化
				char x;
				x = i + 48;
				square[i] = x;
			}
		}else{
			if(strncmp(recvline,"the rival agree",strlen("the rival agree"))==0){
				char mark[2];
				printf("now play the game and choose O or X\n");
				scanf("%s",mark);
				write(sockfd,mark,strlen(mark));
				if(strncmp(mark,"O",1)==0){
					player[0] = 1;  player[1] = 2; //player0 自己 1是對手, 1是O
				}else if(strncmp(mark,"X",1)==0){
					player[0] = 2; player[1] = 1;
				}
				play = 1;
			}
			if(strncmp(recvline,"you are X",strlen("you are X"))==0){
				char len1[10],len2[10],len[3];
				player[0] = 2; player[1] = 1; play = 1;
			}else if(strncmp(recvline,"you are O",strlen("you are O"))==0){
				char len1[10],len2[10],len[3];
				player[0] = 1; player[1] = 2; play = 1;
			}
		}// else judge!=0,1

		bzero(sendline , MAX_LINE);
	}//while
}

/*採用select的客戶端訊息處理函式*/
void str_cli2(FILE* fp , int sockfd)
{
	int maxfd;
	fd_set rset;
	/*傳送和接收緩衝區*/
	char sendline[MAX_LINE] , recvline[MAX_LINE];

	FD_ZERO(&rset);
	while(1)
	{
		/*將檔案描述符和套接字描述符新增到rset描述符集*/
		FD_SET(fileno(fp) , &rset);	
		FD_SET(sockfd , &rset);
		maxfd = max(fileno(fp) , sockfd) + 1;
		select(maxfd , &rset , NULL , NULL , NULL);
		
		if(FD_ISSET(fileno(fp) , &rset))
		{
			if(fgets(sendline , MAX_LINE , fp) == NULL)
			{
				printf("read nothing~\n");
				close(sockfd); /*all done*/
				return ;
			}//if
			sendline[strlen(sendline) - 1] = '\0';
			write(sockfd , sendline , strlen(sendline));
		}//if

		if(FD_ISSET(sockfd , &rset))
		{
			if(readline(sockfd , recvline , MAX_LINE) == 0)
			{
				
				perror("handleMsg: server terminated prematurely.\n");
				exit(1);			
			}//if
			
			if(fputs(recvline , stdout) == EOF)
			{
				perror("fputs error");
				exit(1);
			}//if
		}//if	
	}//while
}

int main(int argc , char **argv)
{
	/*宣告套接字和連結伺服器地址*/
    int sockfd;
    struct sockaddr_in servaddr;

    /*判斷是否為合法輸入*/
    if(argc != 2)
    {
        perror("usage:tcpcli <IPaddress>");
        exit(1);
    }//if

    /*(1) 建立套接字*/
    if((sockfd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
    {
        perror("socket error");
        exit(1);
    }//if

    /*(2) 設定連結伺服器地址結構*/
    bzero(&servaddr , sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if(inet_pton(AF_INET , argv[1] , &servaddr.sin_addr) < 0)
    {
        printf("inet_pton error for %s\n",argv[1]);
        exit(1);
    }//if

    /*(3) 傳送連結伺服器請求*/
    if(connect(sockfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0)
    {
        perror("connect error");
        exit(1);
    }//if
    printf("please enter password : \n");
	/*呼叫普通訊息處理函式*/
	str_cli(sockfd);	
	/*呼叫採用select技術的訊息處理函式*/
	//str_cli2(stdin , sockfd);
	exit(0);
}
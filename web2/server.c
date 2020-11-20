#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    int sockfd=socket(PF_INET,SOCK_STREAM,0);
    /*
    1.[domain]
    PF_INET ：IPv4 協定
    PF_INET6：IPv6 協定
    PF_UNIX 表示本地套接字（使用一個文件）
    2. [type]
    SOCK_STREAM：使用 TCP 
    SOCK_DGRAM： 使用 UDP 
    3.[protocol]指定實際使用的傳輸協定。
    在<netinet/in.h>中有詳細說明。
    如果0的話，即根據選定的domain和type選擇使用默認協定。
    */
    if(sockfd==-1){
       perror("Socket Create Error!\n");
       exit(-1);
    }
    printf("Socket fd=%d\n",sockfd);

    //設定位址資訊的資料 
    struct sockaddr_in addr;
    /*
    struct sockaddr_in：IP4 格式使用
    struct sockaddr_in6：IP6 格式使用
    struct sockaddr：通用格式
    struct sockaddr_un：UNIX domain 格式 
    */
   	memset(&addr, 0, sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(20000);//設定 port
    addr.sin_addr.s_addr=htonl(INADDR_ANY);//inet_addr("127.0.0.1");//設定 IP
    //addr.sin_addr.s_addr = INADDR_ANY; 若設定INADDR_ANY 表示任何IP
    
    int result;
    result=bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));
    /*
    int sockfd：socket() 函式回傳的 socket descriptor
    struct sockaddr *addr：用來通訊的位址資料(IP、PORT)
    int addrlen：位址長度 ，sizeof(addr)
    發生錯誤回傳-1，否則為0
    */
    if(result==-1){
    printf("Bind error!\n");
    close(sockfd);
    exit(-1);
    }
    printf("Bind successfully.\n");


    if ( listen(sockfd, 5) == -1) {
		perror("listen failed.");
		exit(EXIT_FAILURE);
	}
	printf("listen done\n");


    char buf[255];
    struct sockaddr_in from;
    socklen_t len;
    len=sizeof(from);
	 
    int selectReturn;
    fd_set rfd;  
    struct timeval timeout= {3,0};

    while(1){
       FD_ZERO(&rfd); 
       FD_SET(sockfd, &rfd);
       selectReturn = select(sockfd+1, &rfd, NULL, NULL, &timeout);
   //    now_socket = accept(sockfd,(struct sockaddr*) &clientInfo, &addrlen);
       if(selectReturn == -1){  
          printf("select()/n");  
          close(sockfd);
          exit(-1);
       }else{
          if(FD_ISSET(sockfd, &rfd)) {  
             result = recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&from,&len);
             if(result>0){
                buf[result]=0;
                printf("The message received from %s  Data is :%s\n",inet_ntoa(from.sin_addr),buf);
                //inet_ntoa()：將二進位的數字轉換為IP
             }else{
                break;
             }
          }
       }
     }
     close(sockfd);
     return 0;
}
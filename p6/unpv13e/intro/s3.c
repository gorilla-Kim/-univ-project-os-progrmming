#include	"unp.h"
#include	<time.h>
#include    <stdlib.h>

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
    socklen_t           len;
	struct sockaddr_in	servaddr, cliaddr;
	char				buff[MAXLINE], recvline[MAXLINE+1];
	time_t				ticks;
    
    if(argc!=2)
        err_quit("usage: a.out<IPaddress>");
    if(atoi(argv[1])<=1024)
        err_sys("Well known port number error");
    
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));	/* daytime server */

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for ( ; ; ) {
        len = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &len);
        printf("[%s:%d] is connected!!\n", Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
        
        /* 데이터 전송구간 */
        printf("send> ");
        // if(Fgets(buff,MAXLINE,stdin)==NULL) // Ctrl + D 입력시 종료
        //     printf("Ctrl + D 입력!!.\n");
        srand(time(NULL));                // 난수 발생
        sprintf(buff,"%d",rand()%999+1);  // 난수 초기화
        printf("%s\n",buff);              // 난수 화면에 출력
        Writen(connfd, buff, strlen(buff)); // 입력한 값 전송
        
        /* 데이터 받는구간 */
        if(Readline(connfd, recvline, MAXLINE)!=0){ // 읽어온 행의 갯수가 0이 아니라면, 읽어온 값을 화면에 출력!!
            printf("recv> ");
            Fputs(recvline, stdout);
        }
        
        // ticks = time(NULL);
        // snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        // Write(connfd, buff, strlen(buff));

		Close(connfd);
        printf("[%s:%d] is disconnected!!\n\n", Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
	}
}

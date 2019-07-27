#include "unp.h"

#define CHATDATA 1024

char quit[] ="/quit";

int
main(int argc, char *argv[])
{
	struct sockaddr_in c_addr;		// connect addr;

	int c_socket;				// connect socket;
	int maxfd;				// maxfd
	int len;				// len
	int n;					// n
    int stdineof = 0;
    
	char chatData[CHATDATA];		// chat data
	char chatName[CHATDATA];		// chat name
	char buf[CHATDATA];			// buf

	fd_set rset;

	// argv input
	if(argc != 4)
		err_quit("usage: tcpcli <Ipaddress> <Port> <Chat name>");


	// <-- connect socket
	c_socket = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&c_addr, sizeof(c_addr));

	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(atoi(argv[2]));
	Inet_pton(AF_INET, argv[1], &c_addr.sin_addr);

	strcpy(chatName, argv[3]);

	Connect(c_socket, (SA *) &c_addr, sizeof(c_addr));
	// connect socket--> 



	// chatName -> send
	write(c_socket, chatName, strlen(chatName));


	// maxfd -> initialize
	// maxfd = c_socket + 1;



	for(;;)
	{
		FD_ZERO(&rset);
		if(stdineof == 0)
            FD_SET(fileno(stdin), &rset);				// standard input
		FD_SET(c_socket, &rset);			// from chat server
		maxfd = max(fileno(stdin), c_socket)+1;
		Select(maxfd, &rset, NULL, NULL, NULL);		

		if(FD_ISSET(c_socket, &rset))
		{
			memset(chatData, 0, sizeof(chatData));
			if( (n = read(c_socket, chatData, sizeof(chatData))) > 0 )
			{
				write(1, chatData, n);				// standard output 
			}else if( n == 0 ){
                if (stdineof == 1)
					break;
				else
					err_quit("str_cli:server terminated");
            }
		}

		if(FD_ISSET(fileno(stdin), &rset))
		{
			memset(buf, 0, sizeof(buf));
			if( (n = read(0, chatName, sizeof(chatName))) > 0 ) 
			{
				write(c_socket, chatName, n);			// input -> c_socket send

				if(strncmp(chatName, quit, strlen(quit)) == 0)	// quit
				{
                    shutdown(c_socket, SHUT_WR);
                    FD_CLR(fileno(stdin), &rset);
					break;	
				}
			}else if(n == 0 ){
				stdineof = 1;
                shutdown(c_socket, SHUT_WR);
				FD_CLR(fileno(stdin), &rset);
				continue;
            }
		}
	}

	Close(c_socket);
	
}

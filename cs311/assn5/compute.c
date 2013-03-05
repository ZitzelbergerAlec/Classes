/* Includes */
#include 	<ctype.h>
#include 	<errno.h>
#include 	<getopt.h>
#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include  	<strings.h>     /* for bzero */
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/select.h>
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>

#define LISTENQ 1024
#define MAXLINE 4096
#define MAXSOCKADDR 128
#define BUFFSIZE 4096

#define SERV_PORT 3283 // "DAVE" in keypad numbers
#define SERV_PORT_STR "3283"

/* Function prototypes */
int is_perfect(int test_number);

int main(int argc, char **argv)
{
	/* Set up sockets */
	int i;
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE];
	char recvline[MAXLINE];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr); //Segfaults here because no argv

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	
	while(fgets(sendline, MAXLINE, stdin) != NULL){
		
		bzero(recvline, MAXLINE);

		write(sockfd, sendline, strlen(sendline) + 1);

		if(read(sockfd, recvline, MAXLINE) == 0){
			perror("Something broke");
			exit(-1);
		}
		
		fputs(recvline, stdout);

	}

	/* End socket setup code */
	return 0;
}

int is_perfect(int test_number)
{	
	int i;
	int sum = 1;
	for(i=2; i<test_number; i++){
		if((test_number % i) == 0){
			sum += i;
		}
	}
	if(sum == test_number)
		return 1;
	return 0;
}
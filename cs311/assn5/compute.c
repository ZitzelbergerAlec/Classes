/* Compiler directives */

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

/* Global variables and definitions */
#define LISTENQ 1024
#define MAXLINE 4096
#define MAXSOCKADDR 128
#define BUFFSIZE 4096

#define SERV_PORT 43283 // "4DAVE" in keypad numbers
#define SERV_PORT_STR "3283"

typedef struct {
	char request_type[15];
	char sender_name[15];
	float mods_per_sec;
} compute_packet;

/* Function prototypes */
int is_perfect(int test_number);
void send_packet(compute_packet *out_packet, int sockfd);

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
	
	compute_packet mypacket;
	strcpy(mypacket.request_type, "query"); //segfaults here
	strcpy(mypacket.sender_name, "compute");
	mypacket.mods_per_sec = 5000;
	send_packet(&mypacket, sockfd);

	if(read(sockfd, recvline, MAXLINE) == 0){
		perror("Something broke");
		exit(-1);
	}
	
	fputs(recvline, stdout);

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


/* Assembles a packet out of the packet data structure and sends it over the socket */
void send_packet(compute_packet *out_packet, int sockfd)
{
	char temp[MAXLINE]; /* A temp string buffer for the packet */ 
	snprintf(temp, MAXLINE, "<request type=\"%s\"><sender name=\"%s\"/><performance mods_per_sec=\"%f\"/></request>",  out_packet->request_type, out_packet->sender_name, out_packet->mods_per_sec);
	write(sockfd, temp, strlen(temp));
}

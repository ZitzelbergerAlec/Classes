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
#include 	<math.h> /* for sqrt */
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
#define SERV_PORT_STR "43283"

typedef struct {
	char request_type[15];
	char sender_name[15];
	double mods_per_sec;
} compute_packet;

/* Function prototypes */
int is_perfect(int test_number);
int mods_per_sec(int prev_max);
void send_packet(compute_packet *out_packet, int sockfd);

int main(int argc, char **argv)
{
	/* Set up sockets */
	int i;
	int sockfd;
	struct sockaddr_in servaddr;
	char recvline[MAXLINE];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr); //Segfaults here because no argv

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	
	int test_mod_function = mods_per_sec(100000000);
	printf("I can do %d mods per sec\n", test_mod_function);

	/* Assemble and send test packet */
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


/* 
Returns 1 if a test_number is a perfect number.
To do: write this to only do sqrt(test_number) mods
*/
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

/* 
Calculates the number of mod operations capable of per sec.
AKA FLOPS (Floating Point Operations Per Second).
Uses the previous max and the square root of that number for better accuracy.
To do: maybe make an average using an array.
*/
int mods_per_sec(int prev_max)
{
	int sqrt_prev_max = sqrt(prev_max);
	clock_t start, stop;
	start = clock();
	stop = clock();
	int i = 0;
	while((stop-start)/CLOCKS_PER_SEC < 0.05){
		prev_max % sqrt_prev_max; /* Time the mod operation */
		stop = clock();
		i+=2; //Because each loop also divides by CLOCKS_PER_SEC
	}
	return i*20; /* mods per sec */
}


/* Assembles a packet out of the packet data structure and sends it over the socket */
void send_packet(compute_packet *out_packet, int sockfd)
{
	char temp[MAXLINE]; /* A temp string buffer for the packet */ 
	snprintf(temp, MAXLINE, "<request type=\"%s\"><sender name=\"%s\"/><performance mods_per_sec=\"%f\"/></request>",  out_packet->request_type, out_packet->sender_name, out_packet->mods_per_sec);
	write(sockfd, temp, strlen(temp));
}

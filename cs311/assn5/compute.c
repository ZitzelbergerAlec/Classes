/* Compiler directives */
#define _POSIX_SOURCE		//for kill

/* Includes */
#include 	<ctype.h>
#include 	<errno.h>
#include 	<getopt.h>
#include 	<libxml/parser.h>
#include 	<libxml/tree.h>
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
#include 	<sys/types.h>
#include	<sys/wait.h>

/* Global variables and definitions */
#ifdef LIBXML_TREE_ENABLED

#define LISTENQ 1024
#define MAXLINE 4096
#define MAXSOCKADDR 128
#define BUFFSIZE 4096

#define SERV_PORT 43283 // "4DAVE" in keypad numbers
#define SERV_PORT_STR "43283"
/* Variable to keep track of compute PID */
pid_t compute_pid;

typedef struct {
	char request_type[15];
	char sender_name[15];	
} packet;

typedef struct {
	char request_type[15];
	char sender_name[15];
	double mods_per_sec;
} compute_packet;

typedef struct {
	unsigned int min;
	unsigned int max;
} range;

/* Function prototypes */
void compute_process();
void do_nothing(unsigned long *k);
range *get_range(char *packet_string);
int is_perfect(int test_number);
unsigned int mods_per_sec();
packet *parse_packet(char *packet_string);
void puke_and_exit(char *error_message);
void request_range(int sockfd, unsigned int prev_max);
void send_handshake(int sockfd);
void send_new_perfect(int n, int sockfd);
void send_packet(char *packet_string, int sockfd);

int main(int argc, char **argv)
{
	/* 
	Fork off compute process 
	Main process will be CnC (Command and Control)
	*/
	pid_t pid;
	switch(pid = fork()){
		case -1: //Oops case
			puke_and_exit("Forking error\n");
		case 0: //Child case
			compute_process();
			break;
		default: //Parent case
			compute_pid = pid;
			break;

	}

	/* Compute CNC stuff */
	int sockfd;
	struct sockaddr_in servaddr;
	char recvline[MAXLINE];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr); 

	/* Connect to manage server */
	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	
	  /* Wait for server handshake */
	if (read(sockfd, recvline, MAXLINE) == 0){
        	puke_and_exit("Error reading line");
    	}
	
    	/* Send client handshake */
	send_packet("<request type=\"handshake\" sender=\"compute_cnc\"></request>\n", sockfd);

	/* Wait for terminate packet from server */
	if (read(sockfd, recvline, MAXLINE) == 0){
        	puke_and_exit("Error reading line");
    	}

    	/* Parse packet to make sure it's the correct one */
    	packet *header = parse_packet(recvline);

    	if(!strcmp(header->sender_name, "manage") && !strcmp(header->request_type, "terminate")){
    		/* Kill compute process and exit this one */
    		kill(compute_pid, SIGQUIT);
    		wait(NULL);
    		exit(1);
    	}
		
	return 0;
}

/* The process for computing perfect numbers */
void compute_process(){
	/* Set up sockets */
	unsigned int i;
	int sockfd;
	struct sockaddr_in servaddr;
	char recvline[MAXLINE];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr); 

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	
	/* Wait for server handshake */
	if (read(sockfd, recvline, MAXLINE) == 0){
        	puke_and_exit("Error reading line");
    	}
	
    	/* Send client handshake */
	send_handshake(sockfd);	

	int prev_max = 0;
 	range *cur_range;

    	clock_t start, stop;
     	while(1){
     		/* Request new range from server */
     		request_range(sockfd, prev_max);

     		/* Wait for server response */
     		if (read(sockfd, recvline, MAXLINE) == 0){
         		puke_and_exit("Error reading line");
     		}
    		
     		cur_range = get_range(recvline);
	 	printf("Calculating perfect numbers from %d to %d\n", cur_range->min, cur_range->max);
	 	start = clock();
	 	for(i=cur_range->min; i<=cur_range->max; i++){
	 		if(is_perfect(i))
	 			send_new_perfect(i, sockfd);
	 	}
	 	stop = clock();
	 	printf("Calculation took %ld sec\n", (stop-start)/CLOCKS_PER_SEC);
	 	prev_max = cur_range->max;
	 }
}

/* 
Returns 1 if a test_number is a perfect number.
*/
int is_perfect(int n)
{	
	if(n == 1) //1 works, but is not a perfect number
		return 0;
	int i;
	int sum = 1;
	for(i=2; i<sqrt(n); i++){
		if((n % i) == 0){
			sum += i;
			sum += n/i;
		}
	}
	if(sum == n) 
		return 1;
	return 0;
}

/* 
Calculates the number of mod operations capable of per sec.
AKA FLOPS (Floating Point Operations Per Second).
Uses the previous max and the square root of that number for better accuracy.
To do: maybe generate a random number between 0 and sqrt(test_max)
*/
unsigned int mods_per_sec()
{	
	unsigned int i, j = 7;
	unsigned long k = 0;
	clock_t start, stop;
	start = clock();
	for(i = 1; i < 100000000; i++){
	        k += j % i;
	}
	stop = clock();
	//do_nothing(&k); //This doesn't help. 
	double time_elapsed = (double)(stop-start) / CLOCKS_PER_SEC;
	printf("k = %ul\n", k);
	unsigned int multiply_coeff = (int) 1.00/time_elapsed;
	return (i-1) * multiply_coeff;
}

/* Literally does nothing */
void do_nothing(unsigned long *k)
{
	k++;
}

/* Sends a packet string over the socket */
void send_packet(char *packet_string, int sockfd)
{
	write(sockfd, packet_string, strlen(packet_string));
}

/* 
Sends initial handshake to server 
To do: add performance characteristics
*/
void send_handshake(int sockfd)
{
	send_packet("<request type=\"handshake\" sender=\"compute\"><performance mods_per_sec=\"9000\"/></request>\n", sockfd);
}

/*
Sends a new perfect number n to the manage process 
*/
void send_new_perfect(int n, int sockfd)
{
	char temp[MAXLINE]; /* A temp string buffer for the packet */ 
	snprintf(temp, MAXLINE, "<request type=\"new_perfect\" sender=\"compute\"><new_perfect value=\"%d\"/></request>\n", n);
	write(sockfd, temp, strlen(temp));
}

/*
Returns a packet struct containing sender name and request type
*/
packet *parse_packet(char *packet_string)
{
	/* Reference: 
		libxml tutorial: www.xmlsoft.org/tutorial/xmltutorial.pdf
	*/
	xmlDoc *doc = NULL; 
	packet *new_packet = malloc(sizeof(packet));
	xmlNode *request = NULL;
	xmlNodePtr sender;
	
	doc = xmlReadMemory(packet_string, strlen(packet_string), "noname.xml", NULL, 0);
	request = xmlDocGetRootElement(doc);
	if(request == NULL)
		return NULL;
		
	strcpy(new_packet->request_type, (char *) xmlGetProp(request, "type"));
	strcpy(new_packet->sender_name, (char *) xmlGetProp(request, "sender"));
	xmlFreeDoc(doc);
	return new_packet;
}

/* sends a request for a range then  waits for the response */
void request_range(int sockfd, unsigned int prev_max)
{
	char temp[MAXLINE]; /* A temp string buffer for the packet */ 
	snprintf(temp, MAXLINE, "<request type=\"request_range\" sender=\"compute\"><performance mods_per_sec=\"%u\"/><prev_max value=\"%d\"/></request>\n", mods_per_sec(), prev_max);
	write(sockfd, temp, strlen(temp));
}

/* 
Extracts the range out of a range packet and returns it as a struct.
*/
range *get_range(char *packet_string)
{
	/* Reference: 
		libxml tutorial: www.xmlsoft.org/tutorial/xmltutorial.pdf
	*/
	xmlDoc *doc = NULL; 
	range *new_range = malloc(sizeof(range));
	xmlNode *request = NULL;
	xmlNodePtr cur = NULL;
	
	doc = xmlReadMemory(packet_string, strlen(packet_string), "noname.xml", NULL, 0);
	request = xmlDocGetRootElement(doc);
	if(request == NULL)
		return NULL;
	cur = request->xmlChildrenNode;
	
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "min"))) {
			new_range->min = atoi((char *) xmlGetProp(cur, "value"));
		}
		if ((!xmlStrcmp(cur->name, (const xmlChar *) "max"))) {
			new_range->max = atoi((char *) xmlGetProp(cur, "value"));
		}
		cur = cur->next;
	}
	xmlFreeDoc(doc);
	return new_range;
}

void puke_and_exit(char *error_message)
{
	perror(error_message);
	printf("Errno = %d\n", errno);
	exit(EXIT_FAILURE);
}


#else
int main(void) {
    fprintf(stderr, "Tree support not compiled in\n");
    exit(1);
}
#endif

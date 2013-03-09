/* Compiler directives */

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
#include	<sys/wait.h>

/* Global variables and definitions */
#ifdef LIBXML_TREE_ENABLED

#define LISTENQ 1024
#define MAXLINE 4096
#define MAXSOCKADDR 128
#define BUFFSIZE 4096

#define SERV_PORT 43283 // "4DAVE" in keypad numbers
#define SERV_PORT_STR "43283"

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
	int min;
	int max;
} range;

/* Function prototypes */
range *get_range(char *packet_string);
int is_perfect(int test_number);
int mods_per_sec();
packet *parse_packet(char *packet_string);
void request_range(int sockfd);
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
	
	request_range(sockfd);	
	
	if(read(sockfd, recvline, MAXLINE) == 0){
		perror("Something broke");
		exit(-1);
	}
	
	packet *recv_packet = parse_packet(recvline);
	if(!strcmp(recv_packet->sender_name, "manage") && !strcmp(recv_packet->request_type, "new_range")){
		printf("manage sent us new range.\n");
		range *new_range = get_range(recvline);
		printf("min is %d\n", new_range->min);
		printf("max is %d\n", new_range->max);
	}
		
	/* End socket setup code */
	return 0;
}

/* 
Returns 1 if a test_number is a perfect number.
*/
int is_perfect(int test_number)
{	
	int i;
	int sum = 1;
	for(i=2; i<sqrt(test_number); i++){
		if((test_number % i) == 0){
			sum += i;
			sum += test_number/i;
		}
	}
	if(sum == test_number && test_number != 1) //1 works, but is not a perfect number
		return 1;
	return 0;
}

/* 
Calculates the number of mod operations capable of per sec.
AKA FLOPS (Floating Point Operations Per Second).
Uses the previous max and the square root of that number for better accuracy.
To do: maybe generate a random number between 0 and sqrt(test_max)
*/
int mods_per_sec()
{
	int test_max = 100000000;
	int sqrt_test_max = sqrt(test_max);
	clock_t start, stop;
	start = clock();
	stop = clock();
	int i = 0;
	while((stop-start)/CLOCKS_PER_SEC < 0.05){
		test_max % sqrt_test_max; /* Time the mod operation */
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

	return new_packet;
}

/* sends a request for a range then  waits for the response */
void request_range(int sockfd)
{
	char temp[MAXLINE]; /* A temp string buffer for the packet */ 
	snprintf(temp, MAXLINE, "<request type=\"query\" sender=\"compute\"><performance mods_per_sec=\"9000\"/></request>");
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


#else
int main(void) {
    fprintf(stderr, "Tree support not compiled in\n");
    exit(1);
}
#endif
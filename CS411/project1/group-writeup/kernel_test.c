#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

//Defines the policy to use for this task for testing
#define PROCESS_POLICY SCHED_RR	//Change to SCHED_FIFO to test FIFO scheduling
#define TIMESLICE 0.1

//Struct needed for sched_setscheduler() to function correctly: 
struct sched_param param;

void
main()
{
	int i;
	int j;

	param.sched_priority = sched_get_priority_max(PROCESS_POLICY);
	//A "0" as the first parameter to sched_setscheduler() means 
	//set policy for this process
	if (sched_setscheduler(0, PROCESS_POLICY, &param) == -1) {
		printf("sched_setscheduler broke\n");
		exit(-1);
	}

	unsigned long mask = 8;	/* processors 4 */
	unsigned int len = sizeof (mask);
	//Set the CPU affinity to CPU 0
	if (sched_setaffinity(0, len, &mask) < 0) {
		printf("sched_setaffinity not working boss \n");
		exit(-1);
	}

	pid_t pid;
	clock_t start, stop, start1, stop1;
	printf("START\n");
	double time_elapsed, print_time;
	//Fork off four child processes
	for (i = 0; i < 4; i++) {
		switch (pid = fork()) {
		case -1:	//oops case
			exit(-1);
		case 0:	//child case
			j = 0;
			while (j < 4) {
				//Time the print operation
				start = clock();
				printf("Parent: %d PID: %d Iter: %d\n", i,
				       getpid(), j);
				stop = clock();
				print_time =
				    (double) (stop - start) / CLOCKS_PER_SEC;

				start = clock();
				time_elapsed = 0;
				//Busy-wait until the end of the timeslice
				while ((time_elapsed + print_time) < TIMESLICE) {
					asm("");
					/* ^Assembly language code added to 
					 * prevent compiler from potentially 
					 * optimizing out the following lines
					 */
					stop = clock();
					time_elapsed =
					    (double) (stop -
						      start) / CLOCKS_PER_SEC;
				}
				printf("TIMESLICE %f\n",
				       (time_elapsed + print_time));
				j++;
			}
			_exit(EXIT_SUCCESS);
		default:	//parent case
			break;

		}
	}
	for (i = 0; i < 4; i++) {
		//Reap the souls of our dead children
		wait();
	}
	printf("finished\n");
}

#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define SCHEDULER SCHED_RR
#define TIMESLICE 0.1

struct sched_param param;


void main (){
    int i;
    int j;

    param.sched_priority = sched_get_priority_max(SCHEDULER);
    //printf("%d\n", param.sched_priority);
    if( sched_setscheduler( 0, SCHEDULER, &param ) == -1)
    {
        printf("sched_setscheduler broke\n");
        exit(-1);
    }

    unsigned long mask = 8; /* processors 4 */
    unsigned int len = sizeof(mask);
    if (sched_setaffinity(0, len, &mask) < 0) {
	    printf("sched_setaffinity not working boss \n");
	    exit(-1);
    }

    pid_t pid;
    clock_t start, stop, start1, stop1;
    printf("START\n");
    double time_elapsed, print_time;
    for(i = 0; i<4;i++)
    {
        switch(pid = fork())
        {
            case -1: //oops case
                exit(-1);
            case 0:  //child case
                j = 0;
                while(j < 4)
                {
                    start = clock();
                    printf("Parent: %d PID: %d Iter: %d\n",i,getpid(),j);
                    stop = clock();
                    print_time = (double)(stop-start) / CLOCKS_PER_SEC;

		    start = clock();
		    time_elapsed = 0;
		    while((time_elapsed + print_time) < TIMESLICE)
		    {
		    	asm("");
			stop = clock();
                    	time_elapsed = (double)(stop-start) / CLOCKS_PER_SEC;
		    }
		    printf("TIMESLICE %f\n",(time_elapsed+print_time));

                    //sleep(.1 - time_elapsed - .01);
                    j++;
                }
                _exit(EXIT_SUCCESS);
            default:  //parent case
	    	printf("OMG PARENT!\n");

	    	break;

        }
    }
    //wait();
    for(i =0; i<4; i++)
    {
        wait();
    }
    printf("finished\n");

}

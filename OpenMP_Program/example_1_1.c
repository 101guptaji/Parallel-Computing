/**************************************************************************************************************
          HPCOM - 2014, July 24-26,2014

Example		: omp-hello-world.c
Objective       : OpenMP program to print "Hello World"
                  This example demonstrates the use of
                   omp_get_thread_num()
                   omp_get_num_threads() calls
Input		: Set the number of threads to use by means of the OMP_NUM_THREADS environment variable.
               	  For C shell use command :
			setenv OMP_NUM_THREADS 4
		  For bash shell use command :
			export OMP_NUM_THREADS=4.
Output		: Each thread prints a message "Hello World" and its identifier.	                                            
**************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* Main Program */
int main(int argc,char **argv)
{
	int             threadid, Noofthreads;

 	printf("\n\t\t---------------------------------------------------------------------------");
        printf("\n\t\t Objective : OpenMP program to print unique thread identifier for ");
	printf("\n\t\t each thread using OpenMP PARALLEL directive.");
        printf("\n\t\t..........................................................................\n");
	printf("\n\n\t\t Master thread  prints this before forking the team of thread \n");
	
	 /* Checking for command line arguments */
        if( argc !=2 )
	{
           printf("\t\t Very Few Arguments\n ");
           printf("\t\t Syntax : exec <Threads>\n");
           exit(-1);
        }

        Noofthreads = atoi(argv[1]);
        
        /* Set the number of threads */
	omp_set_num_threads(Noofthreads);
	//omp_set_dynamic(1);
	//printf("Dynamic =%d\n", omp_get_dynamic());
	/* OpenMP Parallel Construct : Fork a team of threads */ 
	#pragma omp parallel private(threadid) 
	{
                /* Obtain the thread id */
		threadid = omp_get_thread_num();
		printf("\n\t\tHello World\n");
		/* Each Thread Prints Its Threadid */
		printf("\n\t\t My thread id is : %d\n", threadid);
	
	} /* All thread join Master thread */

	printf("\n\t\t Master thread  prints this after the end parallel region \n \n");
	return 0;
}

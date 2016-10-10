/*
 * PA3: Multi-Threaded DNS Resolution Solution
 * Fall 2016
 * Michael Xiao
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#include "util.h"
#include "queue.h"

#define MAX_RESOLVER_THREADS 10
#define MIN_RESOLVER_THREADS 2
#define MINARGS 3
#define MAX_INPUT_FILES 10
#define USAGE "<inputFilePath> <outputFilePath>"
#define SBUFSIZE 1025
#define MAX_NAME_LENGTH 1025
#define INPUTFS "%1024s"
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

//Initialize Queue and Output File
queue* q;
FILE* outputfp;

//Keep track of global variables
int SearchComplete = 0; //Number of times the DNS search completes
int numResolvers = 0; //Number of Resolvers Created
int numRequestors = 0; //Number of Requestors Created

//Initialization of Mutexes and Conditions
pthread_mutex_t queueLock; //For the queue
pthread_mutex_t outLock; //For the output file
pthread_mutex_t condLock; //For the pthread_cond__wait and pthread_cond__signal
pthread_cond_t condReq; //Requestor Condition
pthread_cond_t condRes; //Resolver Condition

void* Requestor(void* Inputfile)
{
	char* hostname;
	hostname = malloc(SBUFSIZE*sizeof(char));
	FILE* fp = fopen(Inputfile, "r");

	numRequestors++;
	while (fscanf(fp, INPUTFS, hostname) > 0)
	{
		pthread_mutex_lock(&condLock);
		while(queue_is_full(q)) //Wait if the queue is full, since we can't push to a full queue
				pthread_cond_wait(&condReq, &condLock); //Set the condition variable with the condition lock
				//usleep((rand()% 10)+1);
				//continue;
	 pthread_mutex_unlock(&condLock);

		//Put locks around the queue push, to prevent race conditions and read/write errors.

		pthread_mutex_lock(&queueLock);
		queue_push(q, hostname);
		pthread_mutex_unlock(&queueLock);
		hostname = malloc(SBUFSIZE*sizeof(char));

		pthread_mutex_lock(&condLock); //Signal the Resolver that the Requestor has finished
		pthread_cond_signal(&condRes);
		pthread_mutex_unlock(&condLock);
	}

	//Release locks to allow other pthreads to modify, and close the file.

	pthread_mutex_lock(&outLock);
	free(hostname);
	pthread_mutex_unlock(&outLock);

	fclose(fp);
	printf("Requestor Thread added %d hostname(s) to queue.\n", numRequestors);

	return 0;

}

//The RESOLVER function, which uses DNSLookup to find the IP address of every host name given to us in the files.
//Once the DNS is used, it releases the host name from the queue

void* Resolver()
{
	char* outhostname;
	char firstIP[INET6_ADDRSTRLEN];

	while (!queue_is_empty(q) || !SearchComplete)
	{
		//We check if the queue is empty; and if so, release the output host name.
		pthread_mutex_lock(&condLock);
		while (queue_is_empty(q))
		{
			pthread_cond_wait(&condRes, &condLock);
			if(SearchComplete)
			{
				pthread_mutex_lock(&outLock);
				free(outhostname);
				pthread_mutex_unlock(&outLock);
				return 0;
			}
		}
		pthread_mutex_unlock(&condLock);
		//We put locks around here to prevent race conditions and read/write errors while we pop the host name from the queue.

		pthread_mutex_lock(&queueLock);
		outhostname = queue_pop(q);
		pthread_mutex_unlock(&queueLock);

		numResolvers++;

		//If the DNS isn't found, throw an error, but continue

		if(dnslookup(outhostname, firstIP, sizeof(firstIP)) == UTIL_FAILURE)
		{
			fprintf(stderr, "dnslookup error: %s\n", outhostname);
			strncpy(firstIP, "", sizeof(firstIP));

		}

		//We put locks around the release of the hostname, along with it's IP address.

		pthread_mutex_lock(&outLock);
		fprintf(outputfp, "%s,%s\n", outhostname, firstIP);
		free(outhostname);
		pthread_mutex_unlock(&outLock);

		pthread_mutex_lock(&condLock);
		pthread_cond_signal(&condReq);
		pthread_mutex_unlock(&condLock);
	}
		printf("Resolver Thread added %d hostname(s) to queue.\n", numResolvers);
	  return 0;
}

int main(int argc, char* argv[])
{
		int t;
		int numfiles = (argc - 2);

		//Here, we calculate the number of cores on the machine running the code.

		int numCores = sysconf(_SC_NPROCESSORS_ONLN);

		if(numCores < 2)
		{
			numCores = 2;
			//To satisfy the MIN_SESOLVER_THREADS limit, we set any core # less that 2 to 2.
		}

		//Here, we create the pthreads, and the REQUESTOR threads are always proportional to the number of total files.
		//Don't really need init, Don't know why though...
		//pthread_mutex_init(&queueLock, NULL);
		//pthread_mutex_init(&outLock, NULL);
		//pthread_mutex_init(&condLock,NULL);
		//pthread_cond_init(&condReq, NULL);
		//pthread_cond_init(&condRes, NULL);
		pthread_t req_threads[numfiles];
		pthread_t* res_threads;

		res_threads = malloc (numCores * sizeof(pthread_t));

		//rc is to test for the Race Conditions, and hopefully prevent them, or return the error.

		int rc;

	  if(argc < MINARGS)
	  {
	      fprintf(stderr, "Not Enough Arguments: %d\n", (argc - 1));
		    fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
				return EXIT_FAILURE;
	  }

			//Here, we check if Output file exists, and if not, throw and error.

	  if(argc > MAX_INPUT_FILES)
	  {
			  fprintf(stderr, "Too Many Input Files!\n");
			  fprintf(stderr, "Max Number of Files: %d\n",MAX_INPUT_FILES);
				return EXIT_FAILURE;
	  }

	    q = malloc(sizeof(queue));
			queue_init(q,50);

	  if(access(argv[(argc-1)],F_OK) != -1)
	  {
				outputfp = fopen(argv[argc-1],"w");
	  }
	  else
	  {
				fprintf(stderr,"Error Opening Output File!\n");
				return EXIT_FAILURE;
		}

	  for (t = 0; t < numfiles; t++)
	  {
			if(access(argv[t+1],F_OK) == -1)
			{
		        	fprintf(stderr, "Error Opening Input File!\n");
		        	return EXIT_FAILURE;
	    }
			else
			{
				fopen(argv[t], "r");
			}
	    		rc = pthread_create(&(req_threads[t]), NULL, Requestor, argv[t+1]);
	    		if (rc)
	    		{
	    			printf("Error: %d\n", rc);
	    			exit(EXIT_FAILURE);
	    		}
	   }

	  for (t = 0; t < numCores; t++)
	  {
	    	rc = pthread_create(&(res_threads[t]), NULL, Resolver, NULL);
	    	if (rc)
	    	{
	    			printf("Error: %d\n", rc);
	    			exit(EXIT_FAILURE);
	    	}
	  }

	  for (t = 0; t < numfiles; t++)
	  {
	    	pthread_join(req_threads[t], NULL);
	  }

	  SearchComplete = 1;

	  for (t = 0; t < numCores; t++)
	  {
	    	pthread_join(res_threads[t], NULL);
	  }

		pthread_mutex_destroy(&queueLock);
	  pthread_mutex_destroy(&outLock);
		pthread_mutex_destroy(&condLock);
		pthread_cond_destroy(&condReq);
		pthread_cond_destroy(&condRes);


	  fclose(outputfp);
	  queue_cleanup(q);

	  return 0;
}

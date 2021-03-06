
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "defs.h"
#include "hash.h"

#define SAMPLES_TO_COLLECT   10000000
#define RAND_NUM_UPPER_BOUND   100000
#define NUM_SEED_STREAMS            4

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "Team Name",                  /* Team name */

    "Shafaaf Hossain",                    /* First member full name */
    "998891515",                 /* First member student number */
    "shafaaf.hossain@mail.utoronto.ca",                 /* First member email address */

    "",                           /* Second member full name */
    "",                           /* Second member student number */
    ""                            /* Second member email address */
};

//#define DEBUG
#ifdef DEBUG
# define DEBUG_PRINT(x) printf x 
#else
# define DEBUG_PRINT(x) do{} while(0)
# endif


unsigned num_threads;
unsigned samples_to_skip;

class sample;

class sample {
  unsigned my_key;
 public:
  sample *next;
  unsigned count;

  sample(unsigned the_key){my_key = the_key; count = 0;};
  unsigned key(){return my_key;}
  void print(FILE *f){printf("%d %d\n",my_key,count);}
};

// This instantiates an empty hash table
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned".  
//This hash table is main hash table where rest of the other tables will be merged into.
hash<sample,unsigned> h;

//Hash tables used by each thread separately
hash<sample,unsigned> h1;
hash<sample,unsigned> h2;
hash<sample,unsigned> h3;
hash<sample,unsigned> h4;

//My declared functions
void* processSeeds(void * args);

//Data structure to pass in function arguments in different pthreads
typedef struct functionArgs{
	int threadNumber;
	int start;
	int end;
	int numberOfThreads;
}functionArgs;
 

int  
main (int argc, char* argv[]){
  int i,j,k;
  int rnum;
  unsigned key;
  sample *s;

  // Print out team information
  printf( "Team Name: %s\n", team.team );
  printf( "\n" );
  printf( "Student 1 Name: %s\n", team.name1 );
  printf( "Student 1 Student Number: %s\n", team.number1 );
  printf( "Student 1 Email: %s\n", team.email1 );
  printf( "\n" );
  printf( "Student 2 Name: %s\n", team.name2 );
  printf( "Student 2 Student Number: %s\n", team.number2 );
  printf( "Student 2 Email: %s\n", team.email2 );
  printf( "\n" );

  // Parse program arguments
  if (argc != 3){
    printf("Usage: %s <num_threads> <samples_to_skip>\n", argv[0]);
    exit(1);  
  }
  sscanf(argv[1], " %d", &num_threads); // not used in this single-threaded version
  sscanf(argv[2], " %d", &samples_to_skip);

  // initialize a 16K-entry (2**14) hash of empty lists
  h.setup(14);
  
  if(num_threads == 1)
  {
    
  	//Making threads
 	pthread_t thread1;
	DEBUG_PRINT(("Making 1 thread.\n"));
  	const char *message1 = "Thread 1";
  	int iret1;

	//Adding in function arguments
	functionArgs t1Args;
	t1Args.threadNumber = 1;
	t1Args.start = 0;
	t1Args.end = NUM_SEED_STREAMS; 
	t1Args.numberOfThreads = 1;

  	iret1 = pthread_create(&thread1, NULL, processSeeds, &t1Args );
  	if(iret1)
  	{
		DEBUG_PRINT(("ERROR: Making thread 1\n"));	
  	}

  	DEBUG_PRINT(("pthread_create() for thread 1 returns: %d\n", iret1));
  	pthread_join(thread1, NULL);
  }

  else if(num_threads == 2)
  {	
	DEBUG_PRINT(("Making 2 threads.\n"));

  	//Making threads
 	pthread_t thread1, thread2;
  	const char *message1 = "Thread 1";
  	const char *message2 = "Thread 2";
  	int iret1;
  	int iret2;
	
	//Adding in function arguments
	
	//Thread 1
	functionArgs t1Args;
	t1Args.threadNumber = 1;
	t1Args.start = 0;
	t1Args.end = 2;
	t1Args.numberOfThreads = 2;

	//Thread 2
	functionArgs t2Args;
	t2Args.threadNumber = 2;
	t2Args.start = 2;
	t2Args.end = NUM_SEED_STREAMS; 
	t2Args.numberOfThreads = 2;
	
	//Creating threads
	iret1 = pthread_create(&thread1, NULL, processSeeds, &t1Args);
  	if(iret1)
  	{
		DEBUG_PRINT(("ERROR: Making thread 1\n"));	
  	}

  	iret2 = pthread_create(&thread2, NULL, processSeeds, &t2Args); 
  	if(iret2)
  	{
		DEBUG_PRINT(("ERROR: Making thread 2\n"));	
  	}

  	DEBUG_PRINT(("pthread_create() for thread 1 returns: %d\n", iret1));
  	DEBUG_PRINT(("pthread_create() for thread 2 returns: %d\n", iret2));
  	pthread_join(thread1, NULL);
  	pthread_join(thread2, NULL);
  }

  else if(num_threads == 4)
  {	
	DEBUG_PRINT(("Making 4 threads.\n"));

  	//Making threads
 	pthread_t thread1, thread2, thread3, thread4;
  	int iret1;
  	int iret2;
  	int iret3;
  	int iret4;
	
	//Adding in function arguments
	
	//Thread 1
	functionArgs t1Args;
	t1Args.threadNumber = 1;
	t1Args.start = 0;
	t1Args.end = 1;
	t1Args.numberOfThreads = 4;

	//Thread 2
	functionArgs t2Args;
	t2Args.threadNumber = 2;
	t2Args.start = 1;
	t2Args.end = 2;
	t2Args.numberOfThreads = 4;

	//Thread 3
	functionArgs t3Args;
	t3Args.threadNumber = 3;
	t3Args.start = 2;
	t3Args.end = 3;
	t3Args.numberOfThreads = 4;

	//Thread 4
	functionArgs t4Args;
	t4Args.threadNumber = 4;
	t4Args.start = 3;
	t4Args.end = NUM_SEED_STREAMS;
	t4Args.numberOfThreads = 4;
	
	//Creating threads
	iret1 = pthread_create(&thread1, NULL, processSeeds, &t1Args);
  	if(iret1)
  	{
		DEBUG_PRINT(("ERROR: Making thread 1\n"));	
  	}

  	iret2 = pthread_create(&thread2, NULL, processSeeds, &t2Args); 
  	if(iret2)
  	{
		DEBUG_PRINT(("ERROR: Making thread 2\n"));	
  	}

	iret3 = pthread_create(&thread3, NULL, processSeeds, &t3Args);
  	if(iret3)
  	{
		DEBUG_PRINT(("ERROR: Making thread 3\n"));	
  	}

	iret4 = pthread_create(&thread4, NULL, processSeeds, &t4Args);
  	if(iret4)
  	{
		DEBUG_PRINT(("ERROR: Making thread 4\n"));	
  	}
  	
	DEBUG_PRINT(("pthread_create() for thread 1 returns: %d\n", iret1));
  	DEBUG_PRINT(("pthread_create() for thread 2 returns: %d\n", iret2));
  	pthread_join(thread1, NULL);
  	pthread_join(thread2, NULL);
  	pthread_join(thread3, NULL);
  	pthread_join(thread4, NULL);
  }
  else
 {
	DEBUG_PRINT(("Weird input!!\n"));
 }

  //to store element in indivitual hash table
  sample *sTemp;

  for(i = 0;i<RAND_NUM_UPPER_BOUND;i++)
  {
	if(num_threads == 1)
	{
		//check if the random number exists in h1
		if (sTemp = h1.lookup(i))
		{
			//check if random number already exists in h (main hash table)
			if (!(s = h.lookup(i))){
				// insert a new element for it into the hash table
				s = new sample(i);
				h.insert(s);
			 }
			// update the count for the sample
			s->count += sTemp->count;
		}
  	}

	else if(num_threads == 2)
	{
		//check if the random number exists in h1
		if (sTemp = h1.lookup(i))
		{
			//check if random number already exists in h (main hash table)
			if (!(s = h.lookup(i))){
				// insert a new element for it into the hash table
				s = new sample(i);
				h.insert(s);
			 }
			// update the count for the sample
			s->count += sTemp->count;
		}

		//check if the random number exists in h2
		if (sTemp = h2.lookup(i))
		{
			//check if random number already exists in h (main hash table)
			if (!(s = h.lookup(i))){
				// insert a new element for it into the hash table
				s = new sample(i);
				h.insert(s);
			 }
			// update the count for the sample
			s->count += sTemp->count;
		}
  	}

	else if(num_threads == 4)
	{
		//check if the random number exists in h1
		if (sTemp = h1.lookup(i))
		{
			//check if random number already exists in h (main hash table)
			if (!(s = h.lookup(i))){
				// insert a new element for it into the hash table
				s = new sample(i);
				h.insert(s);
			 }
			// update the count for the sample
			s->count += sTemp->count;
		}

		//check if the random number exists in h2
		if (sTemp = h2.lookup(i))
		{
			//check if random number already exists in h (main hash table)
			if (!(s = h.lookup(i))){
				// insert a new element for it into the hash table
				s = new sample(i);
				h.insert(s);
			 }
			// update the count for the sample
			s->count += sTemp->count;
		}

		//check if the random number exists in h3
		if (sTemp = h3.lookup(i))
		{
			//check if random number already exists in h (main hash table)
			if (!(s = h.lookup(i))){
				// insert a new element for it into the hash table
				s = new sample(i);
				h.insert(s);
			 }
			// update the count for the sample
			s->count += sTemp->count;
		}
		//check if the random number exists in h4
		if (sTemp = h4.lookup(i))
		{
			//check if random number already exists in h (main hash table)
			if (!(s = h.lookup(i))){
				// insert a new element for it into the hash table
				s = new sample(i);
				h.insert(s);
			 }
			// update the count for the sample
			s->count += sTemp->count;
		}
  	}
	else
	{
		printf("Weird case\n");	
	}

  }



  h.print();
 
  DEBUG_PRINT(("Ending main program\n"));

}

void* processSeeds(void* args)
{
  
  //Get start and ending seed numbers
  functionArgs * myArgs = (functionArgs*) args;
  int threadNumber = myArgs->threadNumber;
  int startSeed = myArgs->start;
  int endSeed = myArgs->end;
  int numberOfThreads = myArgs->numberOfThreads;;

  DEBUG_PRINT(("Thread %d: My pid is %u, number of threads is %d, start seed is %d and end seed is %d\n",threadNumber, (unsigned int)pthread_self(), numberOfThreads, startSeed, endSeed));

  
  //Pointer to hash table this thread will work with
  hash<sample,unsigned> * threadHashTable;
  
  //initialize a 16K-entry (2**14) hash of empty lists
  if(numberOfThreads == 1) 
  {
	//1 thread so only use first hash table
	h1.setup(14);
	threadHashTable = &h1;
  }

//initialize a 8K-entry (2**13) hash of empty lists for eac
  else if(numberOfThreads == 2) 
  {
	if(threadNumber == 1)
	{
		h1.setup(13);
		threadHashTable = &h1;
	}
	else if(threadNumber == 2)
	{
		h2.setup(13);
		threadHashTable = &h2;
	}
  }

//initialize a 4K-entry (2**12) hash of empty lists
  else if(numberOfThreads == 4) 
  {
	if(threadNumber == 1)
	{
		h1.setup(12);		
		threadHashTable = &h1;
	}
	else if(threadNumber == 2)
	{
		h2.setup(12);
		threadHashTable = &h2;
	}

	else if(threadNumber == 3)
	{
		h3.setup(12);		
		threadHashTable = &h3;
	}

	else if(threadNumber == 4)
	{
		h4.setup(12);		
		threadHashTable = &h4;
	}
  }
  else
  {
	printf("Weird!! Should not come here!");
  }

  int i,j,k;
  int rnum;
  unsigned key;
  sample *s;

  // process streams starting with different initial numbers
  for (i=startSeed; i<endSeed; i++)
  {
    rnum = i;

    // collect a number of samples
    for (j=0; j<SAMPLES_TO_COLLECT; j++){

      // skip a number of samples
      for (k=0; k<samples_to_skip; k++){
	rnum = rand_r((unsigned int*)&rnum);
      }

      // force the sample to be within the range of 0..RAND_NUM_UPPER_BOUND-1
      key = rnum % RAND_NUM_UPPER_BOUND;

      // if this sample has not been counted before
      //if (!(s = h.lookup(key)))
      if(!(s = threadHashTable->lookup(key)))
      {
	
	// insert a new element for it into the hash table
	s = new sample(key);
	//h.insert(s);
	threadHashTable->insert(s);
      }

      // increment the count for the sample
      s->count++;
    
     //DEBUG_PRINT(("Thread: %d - Random number for seed %d and sample no. %d is: %d\n", threadNumber, i, j, rnum));
    }

     //print a list of the frequency of all samples
     //threadHashTable->print();
  }

}


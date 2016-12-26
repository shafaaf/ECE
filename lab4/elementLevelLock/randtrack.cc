
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
  pthread_mutex_t elementLevelMutex;

  //Constructor
  sample(unsigned the_key){
	my_key = the_key; 
	count = 0;
	elementLevelMutex = PTHREAD_MUTEX_INITIALIZER;	
};
  unsigned key(){return my_key;}
  void print(FILE *f){printf("%d %d\n",my_key,count);}
};

// This instantiates an empty hash table
// it is a C++ template, which means we define the types for
// the element and key value here: element is "class sample" and
// key value is "unsigned".  
hash<sample,unsigned> h;


//My declared functions
void* processSeeds(void * args);

//Data structure to pass in function arguments in different pthreads
typedef struct functionArgs{
	int threadNumber;
	int start;
	int end;
}functionArgs;
 
//--------------------------------------------------------------------------------------------

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
  
  //Setup locks in hash.h
  listLevelMutexInit();

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

	//Thread 2
	functionArgs t2Args;
	t2Args.threadNumber = 2;
	t2Args.start = 2;
	t2Args.end = NUM_SEED_STREAMS; 
  	
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

	//Thread 2
	functionArgs t2Args;
	t2Args.threadNumber = 2;
	t2Args.start = 1;
	t2Args.end = 2;

	//Thread 3
	functionArgs t3Args;
	t3Args.threadNumber = 3;
	t3Args.start = 2;
	t3Args.end = 3;

	//Thread 4
	functionArgs t4Args;
	t4Args.threadNumber = 4;
	t4Args.start = 3;
	t4Args.end = NUM_SEED_STREAMS;

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

  //print a list of the frequency of all samples
  h.print();
  DEBUG_PRINT(("Ending main program\n"));

}

//--------------------------------------------------------------------------------------------

void* processSeeds(void* args)
{
 
  //Get start and ending seed numbers
  functionArgs * myArgs = (functionArgs*) args;
  int threadNumber = myArgs->threadNumber;
  int startSeed = myArgs->start;
  int endSeed = myArgs->end;

  DEBUG_PRINT(("Thread %d: My pid is %u, and start seed is %d and end seed is %d\n",threadNumber, (unsigned int)pthread_self(), startSeed, endSeed));

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


      //If new sample needs to be inserted need to lock insert
      //If sample already exists, only lock that element counter
      
      //If this sample has not been counted before
      if (!(s = h.lookup(key))){	//lookup() sends back element in proper list in proper hash index
	
	// insert a new element for it into the hash table
	s = new sample(key);
	
	//Didnt insert as different thread inserted quick times, so delete that object just made
	if(h.insert(s) == 0)
	{
		delete s;	
	}
      }


      // lock, increment the count for the sample and unlock
      pthread_mutex_lock(&(s->elementLevelMutex)); 
      s->count++;
      pthread_mutex_unlock(&(s->elementLevelMutex)); 

     DEBUG_PRINT(("Thread: %d - Random number for seed %d and sample no. %d is: %d\n", threadNumber, i, j, rnum));
    }
  }

}

//--------------------------------------------------------------------------------------------------



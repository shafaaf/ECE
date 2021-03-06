
#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include "list.h"
#include <pthread.h>

//#define DEBUGH
#ifdef DEBUGH
# define DEBUGH_PRINT(x) printf x 
#else
# define DEBUGH_PRINT(x) do{} while(0)
# endif

// Setup mutexes for list-level locks
// my mutex locks for every list. There are 2^16 lists
#define NUM_HASH_INDEXES 16384

pthread_mutex_t listMutex[NUM_HASH_INDEXES];
void listLevelMutexInit (void)
{
	DEBUGH_PRINT(("Hash.h: Initializing mutexes.\n"));
	int i;
	for(i=0; i<NUM_HASH_INDEXES; i++)
	{
		listMutex[i] = PTHREAD_MUTEX_INITIALIZER;
	}
}


#define HASH_INDEX(_addr,_size_mask) (((_addr) >> 2) & (_size_mask))

template<class Ele, class Keytype> class hash;

template<class Ele, class Keytype> class hash {
 private:
  unsigned my_size_log;
  unsigned my_size;
  unsigned my_size_mask;
  list<Ele,Keytype> *entries;
  list<Ele,Keytype> *get_list(unsigned the_idx);

 public:
  void setup(unsigned the_size_log=5);
  int insert(Ele *e);
  Ele *lookup(Keytype the_key);
  void lockList(Keytype the_key);
  void unlockList(Keytype the_key); 
  void print(FILE *f=stdout);
  void reset();
  void cleanup();
};

//Sets up hash table
template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::setup(unsigned the_size_log){
  my_size_log = the_size_log;
  my_size = 1 << my_size_log;
  my_size_mask = (1 << my_size_log) - 1;
  entries = new list<Ele,Keytype>[my_size];
}

//Returns list at the hash index
template<class Ele, class Keytype> 
list<Ele,Keytype> *
hash<Ele,Keytype>::get_list(unsigned the_idx){
  if (the_idx >= my_size){
    fprintf(stderr,"hash<Ele,Keytype>::list() public idx out of range!\n");
    exit (1);
  }
  return &entries[the_idx];
}

//returns element in proper list which is at proper hash index
template<class Ele, class Keytype> 
Ele *
hash<Ele,Keytype>::lookup(Keytype the_key){
  list<Ele,Keytype> *l;

  l = &entries[HASH_INDEX(the_key,my_size_mask)];
  return l->lookup(the_key);
}  

//--------------------------------------------------------------------------------

//My lock function to lock a specific list based on key
template<class Ele, class Keytype> 
void
hash<Ele,Keytype>::lockList(Keytype the_key){
	int lockListKey = HASH_INDEX(the_key,my_size_mask);
	DEBUGH_PRINT(("hash.h: Want to lock a list at index: %d\n", lockListKey));
	pthread_mutex_lock(&(listMutex[lockListKey]));
  	return;
}  


//My unlock function to unlock a specific list based on key
template<class Ele, class Keytype> 
void
hash<Ele,Keytype>::unlockList(Keytype the_key){
	int unlockListKey = HASH_INDEX(the_key,my_size_mask);
	DEBUGH_PRINT(("hash.h: Want to unlock a list at index: %d\n", unlockListKey));
	pthread_mutex_unlock(&(listMutex[unlockListKey]));
  	return;
} 

//--------------------------------------------------------------------------------

//Print hash table and each list
template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::print(FILE* f){  
  unsigned i;
  for (i=0;i<my_size;i++){
    entries[i].print(f);
  }
}

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::reset(){
  unsigned i;
  for (i=0;i<my_size;i++){
    entries[i].cleanup();
  }
}

template<class Ele, class Keytype> 
void 
hash<Ele,Keytype>::cleanup(){
  unsigned i;
  reset();
  delete [] entries;
}

//First lookup with lock and then insert if can or else dont
//finds list in proper hashtable index and inserts in there
template<class Ele, class Keytype> 
int
hash<Ele,Keytype>::insert(Ele *e){
  //unsigned key = 
  lockList(e->key());
   
  //Insert only if not already present
  if (!(lookup(e->key())))
  {
  	entries[HASH_INDEX(e->key(),my_size_mask)].push(e);
  	unlockList(e->key());
	return 1;
  }
  else	//something already present there
  { 		
	unlockList(e->key());
	return 0;
  }
  
}


#endif

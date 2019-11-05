/*******************************************************************
 *File: scheduling_policy.c
 *Author: Prashant Yadav
 *Procedure:
 *clear_cache - It accepts struct of cache type and clear all intialized memory locations.
 *initialize_cache - Before starting the simulation it initializes all four type of caches.
 *uniform - It generates numbers uniformly between given high and low values.
 *normal - it generates normally distributed numbers which are used as arbitrary memory addresses.
 *generatePageAddresses - Before each simulation it populates address array with arbitrary memory addresses.
 *print_cache - Given a struct type cache it prints all cache location.
 *get_memory_loc - Given a struct type cache and memory address it returns memory location or null. 
 *lru_policy_faults - It accepts working set size, it simulates memory allocation process for LRU eviction policy and returns number of faults.
 *fifo_policy_faults - It accepts working set size, it simulates memory allocation process for FIFO eviction policy and returns number of faults.
 *random_policy_faults - It accepts working set size, it simulates memory allocation process for RANDOM eviction policy and returns number of faults.
 *clock_policy_faults - It accepts working set size, it simulates memory allocation process for CLOCK eviction policy and returns number of faults.
 *simulatePageFaults - This method simulates page fault process from working set size 2 to 19.
 *inializeFaultMatrix - Before starting simulations this method initiallizes fault matrix with all zeros.
 *printFaultMatrix - This routine is used to print fault matrix.
 *normalizeFaultMatrix - Normalizes fault matrix for given number of experiments.
 *main - Driver main routine from where program starts executing. In this routine we simulate memory allocation process for 1000 experiments.
 *******************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

//19 rows to store faults from working set size from 2 to 19.
//Each column stores fault size for LRU, FIFO, CLOCK, RANDOM eviction policy.
//index0 - LRU, index1 - FIFO, index2 - CLOCK, index3 - RANDOM 
int faults[19][4];
int experiments=1000, stream_length=1000;  //Number of experiments and address stream length
int address[1000];

//This structure is representation of single memory location in cache.
struct memory_location{
  int address;
  struct memory_location *next;
  struct memory_location *prev;
  int use;           //Only Used in Clock policy scheduling.
};

//This structure is representation of a cache.
struct cache{
  int cache_size;   //It shows current cache size.
  struct memory_location *start;   //Dummy starting memory location.
  struct memory_location *end;	   //Dummy end memory location.
};

//These are the four variables used for each type of cache.
struct cache lru_cache, fifo_cache, clock_cache, random_cache;

/*******************************************************************
 *void clear_cache(struct cache *c)
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: it clears given cache after each simulation for a working set.
 *Parameters:
 *struct cache *c I/P: This is the cache which needs to be cleared.
 *This routine does not return anything.
 *******************************************************************/
void clear_cache(struct cache *c){
  c->cache_size=0;
  c->start = (struct memory_location*)malloc(sizeof(struct memory_location));
  c->end   = (struct memory_location*)malloc(sizeof(struct memory_location));
  c->start->address=-1;
  c->end->address=-1;
  c->start->use=0;
  c->end->use=0;
  c->start->next = c->end;
  c->end->prev = c->start;
  c->start->prev=NULL;
  c->end->next=NULL;
}

/*******************************************************************
 *void initialize_cache()
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: It initializes or re-initializes all types of caches by consecutively making a call to clear_cache routine.
 *Parameters:
 *This routine does not take any argument.
 *This routine does not return anything.
 *******************************************************************/
void initialize_cache(){
  clear_cache(&lru_cache);
  clear_cache(&fifo_cache);
  clear_cache(&clock_cache);
  clear_cache(&random_cache);
}

/*******************************************************************
 *int uniform(int lo, int hi)
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: It geneates uniformly distributed random numbers between given range.
 *Parameters:
 *int lo I/P: It is lower bound on given ranges
 *int hi I/P: It is upper bound on given ranges
 *This routine does not return anything.
 *******************************************************************/
int uniform(int lo, int hi)
{
  int x, y=(hi-lo)+1,z=RAND_MAX/y;
  while(y<=(x=(random()/z)));
  return x+lo;
}

/*******************************************************************
 *int normal ()
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: It geneates normal distributed random numbers which are used as a arbitrary memory address.
 *Parameters:
 *This routine does not take any argument.
 *This routine does not return anything.
 *******************************************************************/
int normal ()
{
    int x = 0,i;
    for(i=0; i<5; i++ )
    {
	x += uniform( 0, 5 );
    }
    return x;
}

/*******************************************************************
 *int uniform(int lo, int hi)
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: It geneates uniformly distributed random numbers between given range.
 *Parameters:
 *int lo I/P: It is lower bound on given ranges
 *int hi I/P: It is upper bound on given ranges
 *This routine does not return anything.
 *******************************************************************/
void generatePageAddresses(){
    int part,index;
    for( part=0; part<10; part++ )
    {
      int base_addr = 25 * uniform(0,9);
      for( index=0; index <100; index ++ )
      {
	address[100*part+index] = base_addr + normal();
      }
    }
}

/*******************************************************************
 *void print_cache(struct cache c)
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: It prints the given type of cache. This is used to debug the program
 *Parameters:
 *struct cache c I/P: This argument is cache which will be printed.
 *This routine does not return anything.
 *******************************************************************/
void print_cache(struct cache c){
  struct memory_location *temp = c.start->next;
  while(temp!=c.end){
    printf("%d ",temp->address);
    temp=temp->next;
  }
}

/*******************************************************************
 *struct memory_location *  get_memory_loc(struct cache c, int address)
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: It searches for given address in given cache.
 *Parameters:
 *struct cache c I/P: This argument is cache which will be searched for given address.
 *int address I/P: Cache c will be searched for this address.
 *memory_location * O/P: It returns memory location pointer if address exists in cache otherwise returns null.
 *******************************************************************/
struct memory_location *  get_memory_loc(struct cache c, int address){
    struct memory_location *res = NULL;
    if(c.cache_size==0){;
      return res;
    }
    struct memory_location *temp = c.start->next;
    while(temp!=c.end && temp->address!=address){
      temp = temp->next;
    }
    if(temp!=c.end && temp->address==address){
      res = temp;
    }
    return res;
}

/*******************************************************************
 *int lru_policy_faults(int working_set)
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: This routine simulates LRU policy eviction policy for given working set size.
 *Parameters:
 *int working_set I/P: This is the given cache size for which fault count needs to be calculated.
 *int O/P: It returns number of faults for LRU eviction policy.
 *******************************************************************/
int lru_policy_faults(int working_set){
    int i;
    int fault_count=0;
    for(i=0;i<stream_length;i++){
      struct memory_location *location = get_memory_loc(lru_cache, address[i]);
	  //If location is not found and cache size reaches working set remove location from end of the cache
	  //and place this location at the start. 
      if(location==NULL && lru_cache.cache_size==working_set){ 
        fault_count++;
        lru_cache.end->prev->prev->next = lru_cache.end;
	lru_cache.end->prev = lru_cache.end->prev->prev;
	location = (struct memory_location*)malloc(sizeof(struct memory_location));
	location->address = address[i];
	location->next=(struct memory_location*)malloc(sizeof(struct memory_location));
	location->prev=(struct memory_location*)malloc(sizeof(struct memory_location));
	lru_cache.cache_size=lru_cache.cache_size-1;
      }else if(location==NULL){
        location = (struct memory_location*)malloc(sizeof(struct memory_location));
	location->next=(struct memory_location*)malloc(sizeof(struct memory_location));
	location->prev=(struct memory_location*)malloc(sizeof(struct memory_location));
        location->address = address[i];
      }else{
        location->next->prev = location->prev;
	location->prev->next = location->next;
        lru_cache.cache_size = lru_cache.cache_size-1;
      }
      location->next = lru_cache.start->next;
      lru_cache.start->next->prev = location;
      location->prev = lru_cache.start;
      lru_cache.start->next = location;
      lru_cache.cache_size = lru_cache.cache_size+1;
    }
    return fault_count;
}


/*******************************************************************
 *int fifo_policy_faults(int working_set)
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: This routine simulates FIFO policy eviction policy for given working set size.
 *Parameters:
 *int working_set I/P: This is the given cache size for which fault count needs to be calculated.
 *int O/P: It returns number of faults for FIFO eviction policy.
 *******************************************************************/
int fifo_policy_faults(int working_set){
    int fault_count=0,i;
    for(i=0;i<stream_length;i++){
      struct memory_location *location = get_memory_loc(fifo_cache, address[i]);
      if(location!=NULL){ //Location is not null continue.
        continue;
      }
	  //If location is null place that locationn at the end of cache and update cache size
      location = (struct memory_location*)malloc(sizeof(struct memory_location));
      location->address = address[i];
      location->next=(struct memory_location*)malloc(sizeof(struct memory_location));
      location->prev=(struct memory_location*)malloc(sizeof(struct memory_location));
      if(fifo_cache.cache_size==working_set){
        fault_count++;
	fifo_cache.start->next->next->prev = fifo_cache.start;
	fifo_cache.start->next = fifo_cache.start->next->next;
        fifo_cache.cache_size--;
      }
      location->prev = fifo_cache.end->prev;
      fifo_cache.end->prev->next = location;
      location->next = fifo_cache.end;
      fifo_cache.end->prev = location;
      fifo_cache.cache_size++;
    }
    return fault_count;
}

/*******************************************************************
 *int random_policy_faults(int working_set)
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: This routine simulates RANDOM policy eviction policy for given working set size.
 *Parameters:
 *int working_set I/P: This is the given cache size for which fault count needs to be calculated.
 *int O/P: It returns number of faults for RANDOM eviction policy.
 *******************************************************************/
int random_policy_faults(int working_set){
    int fault_count=0,i;
    for(i=0;i<stream_length;i++){
      struct memory_location *location = get_memory_loc(random_cache, address[i]);
      if(location!=NULL){  //Location is found continue.
        continue;
      }
	  //If location not found create location and place it at appropriate location as per Random eviction policy
      location = (struct memory_location*)malloc(sizeof(struct memory_location));
      location->address = address[i];
      location->next=(struct memory_location*)malloc(sizeof(struct memory_location));
      location->prev=(struct memory_location*)malloc(sizeof(struct memory_location));
      if(random_cache.cache_size==working_set){
         fault_count++;
	 int index = 1+(rand()%random_cache.cache_size);
	 struct memory_location * temp = random_cache.start->next;
	 while(index>1){
           index--;
	   temp=temp->next;
	 }
	 temp->prev->next = temp->next;
	 temp->next->prev = temp->prev;
	 random_cache.cache_size--;
      }
      location->next = random_cache.start->next;
      random_cache.start->next->prev = location;
      location->prev = random_cache.start;
      random_cache.start->next = location;
      random_cache.cache_size++;
    }
    return fault_count;
}

/*******************************************************************
 *int clock_policy_faults(int working_set)
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: This routine simulates CLOCK policy eviction policy for given working set size.
 *Parameters:
 *int working_set I/P: This is the given cache size for which fault count needs to be calculated.
 *int O/P: It returns number of faults for CLOCK eviction policy.
 *******************************************************************/
int clock_policy_faults(int working_set){
    int fault_count=0,i;
    struct memory_location *clock = clock_cache.start->next;
    for(i=0;i<stream_length;i++){
      struct memory_location *location = get_memory_loc(clock_cache, address[i]);
      if(location!=NULL){  //If memory not null set its use bit to 1 and continue.
        location->use=1;
	    continue;
      }
	  //If location not found create memory location and place it appropriate location as per clock eviction policy.
      location = (struct memory_location*)malloc(sizeof(struct memory_location));
      location->address = address[i];
      location->next=(struct memory_location*)malloc(sizeof(struct memory_location));
      location->prev=(struct memory_location*)malloc(sizeof(struct memory_location));
      location->use=1;
      if(clock_cache.cache_size==working_set){
        fault_count++;
	while(1){
          if(clock==clock_cache.end){
 	    clock=clock_cache.start->next;
	    continue;
	  }
          if(clock->use==0){
	    break;
	  }
	  clock->use=0;
	  clock=clock->next;
	}
	clock_cache.cache_size--;
      }

      location->prev = clock->prev;
      clock->prev->next = location;
      if(clock==clock_cache.end){
         location->next = clock;
	 clock->prev = location;
      }else{
        location->next = clock->next;
        clock->next->prev = location;
	clock=location->next;
      }
      clock_cache.cache_size++;
    }
    return fault_count;
}

/*******************************************************************
 *void simulatePageFaults()
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: This routine simulates page faults for working set size from 2 to 20.
 *Parameters:
 *This routine does not take any argument.
 *This routine does not return anything.
 *******************************************************************/
void simulatePageFaults()
{
     generatePageAddresses();
     int working_set;
     for(working_set=2;working_set<=20;working_set++){
        initialize_cache();
	faults[working_set-2][0]+=lru_policy_faults(working_set);	  //Simulate LRU eviction policy
	faults[working_set-2][1] += fifo_policy_faults(working_set);  //Simulate FIFO eviction policy 
	faults[working_set-2][2] += clock_policy_faults(working_set); //Simulate CLOCK eviction policy
	faults[working_set-2][3] += random_policy_faults(working_set);//Simulate RANDOM eviction policy
     }
}

/*******************************************************************
 *void inializeFaultMatrix()
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: This routine initializes fault matrix with 0 before simulation.
 *Parameters:
 *This routine does not take any argument.
 *This routine does not return anything.
 *******************************************************************/
void inializeFaultMatrix(){
  int i;
  for(i=0;i<19;i++){
     faults[i][0]=0;
     faults[i][1]=0;
     faults[i][2]=0;
     faults[i][3]=0;
  }
}

/*******************************************************************
 *void printFaultMatrix()
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: It prints fault matrix.
 *Parameters:
 *This routine does not take any argument.
 *This routine does not return anything.
 *******************************************************************/
void printFaultMatrix(){
  int i;
  printf("\n------------------------------------------------------------\n");
  printf("Eviction policy ->              LRU\tFIFO\tCLOCK\tRANDOM\n");
  for(i=0;i<19;i++){
    printf("Faults for working set %d is - \t%d\t%d\t%d\t%d\n",i+2,faults[i][0],faults[i][1],faults[i][2],faults[i][3]);
  }
  printf("--------------------------------------------------------------\n");
}

/*******************************************************************
 *void normalizeFaultMatrix()
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: This routine normalizes fault matrix for given number of experiments.
 *Parameters:
 *This routine does not take any argument.
 *This routine does not return anything.
 *******************************************************************/
void normalizeFaultMatrix(){
  int i;
  for(i=0;i<19;i++){
    faults[i][0]/=experiments;
    faults[i][1]/=experiments;
    faults[i][2]/=experiments;
    faults[i][3]/=experiments;
  }
}

/*******************************************************************
 *void main()
 *Author: Prashant Yadav
 *Date: 10/28/2019
 *Description: This is the driver method which simulates fault calculation process for 1000 experiments.
 *Parameters:
 *This routine does not take any argument.
 *This routine does not return anything.
 *******************************************************************/
void main()
{
    int experiment;
    inializeFaultMatrix();  //Initialize fault matrix before starting simulations
    for(experiment=0;experiment<experiments;experiment++)
    {
      printf("Starting Simulation for %d\n",experiment+1);
      simulatePageFaults();	
    }
    normalizeFaultMatrix();	//Normalize fault matrix
    printFaultMatrix();	//Print fault matrix.
}



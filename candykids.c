#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <assert.h>

#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>

#include "bbuff.h"
#include "stats.h"




// Global variables

// Lock
pthread_mutex_t mutex;
sem_t fullLock; 
sem_t emptyLock;

_Bool stop_thread = false;

pthread_t *kid_thread;
pthread_t *factories_thread;
pthread_mutex_t mutex;


//helper function
double current_time_in_ms(void){
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    
    return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}



void* factoryFunc(void* p) {
    int facId = *(int*)p;
 
 
    while (!stop_thread) {
        unsigned int rand_sec = (unsigned int)rand() % 3;  
        candy_t* candy = malloc(sizeof(candy_t));
        candy->factory_number = facId;
        candy->time_stamp_in_ms = current_time_in_ms();
      

        // add lock to the critical section, add candy
        sem_wait(&emptyLock);
        pthread_mutex_lock(&mutex);
    
        bbuff_blocking_insert(candy);
        stats_record_produced(facId);
        printf("\tFactory %d ships candy & waits %ds\n",facId, rand_sec);
        pthread_mutex_unlock(&mutex);
        sem_post(&fullLock);

        // sleep time
        sleep(rand_sec);
    }
    printf("Candy-factory %d done\n", facId);

    //exit the pthread
    pthread_exit(NULL);
}

void* kidFunc(void* p) {
    
    
    candy_t* candy; 
    
    while (true) {
        unsigned int rand_sec = (unsigned int)rand() % 2;  // Either 0 or 1

  
        // add lock to the critical section, eat candy
        sem_wait(&fullLock);
        pthread_mutex_lock(&mutex);
     
        candy = bbuff_blocking_extract();
        stats_record_consumed(candy->factory_number, candy->time_stamp_in_ms);
       
        pthread_mutex_unlock(&mutex);
        sem_post(&emptyLock);

        // sleep time
        sleep(rand_sec);

    }
     //exit the pthread
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    // 1. Extract arguments---------------------
  
    int Fac = atoi(argv[1]);
    int Kid = atoi(argv[2]);
    int Sec = atoi(argv[3]);

    // Check if all input meets requirements

    if(argc != 4) {
        printf("Error, must have 3 arguments\n\n");
        
        return 1;
    }
    // Process arguments

    if (Fac <= 0 || Kid <= 0 || Sec <= 0){
        printf("Error, arguments must be positive integers\n\n");
        return 1;
    }

 
    //2. Initialise modules-----------------


    // Initialise Locks
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("\nError: mutex init failed\n");
        return 1;
    }
    sem_init(&fullLock, 0, 0);
    sem_init(&emptyLock, 0, BUFFER_SIZE);

    // Initialise buffer and thread pointers
    bbuff_init();
    
    int* facIds = malloc(sizeof(int) * (unsigned int)Fac);
    int* kidIds = malloc(sizeof(int) * (unsigned int)Kid);
    factories_thread = malloc(sizeof(pthread_t) * (unsigned int)Fac);
    kid_thread = malloc(sizeof(pthread_t) * (unsigned int)Kid);
    // Initialise stats
    stats_init(Fac);

 
    //3. Launch candy-factory threads----------------
   

    
    for(int i=0; i<Fac; i++) {
        // Create the factory thread 
        facIds[i] = i;
        pthread_create(&factories_thread[i], NULL, factoryFunc, (void *)&facIds[i]);
    }


    //4. Launch kid threads---------------------------
    
    for(int i=0; i<Kid; i++) {
        // Create the kid thread
        kidIds[i] = i;
        pthread_create(&kid_thread[i], NULL, kidFunc, NULL);
    }


    
    //5. Wait for requested time-----------------------
    

    printf("Time 0s\n");
    for (int i=1; i<=Sec; i++){
        sleep(1);
        printf("Time %ds\n", i);
    }


  
    //6. Stop candy-factory threads------------------
   

    stop_thread = true;
    for(int i=0; i<Fac; i++) {
        // stop the factories
        pthread_join(factories_thread[i], NULL);
    }

    free(facIds);
    free(factories_thread);
    printf("Stopping kids\n");
 

    
    //7. Wait until no more candy-------------------
    

    while (!bbuff_is_empty()) {
        printf("Waiting for all candy to be consumed\n");
        sleep(1);
    }
   
   
    //8. Stop kid threads----------------------------
    

    for(int i=0; i<Kid; i++) {
        // stop the kids
        pthread_cancel(kid_thread[i]);
        pthread_join(kid_thread[i], NULL);
    }
  
    free(kidIds);
    free(kid_thread);
 

    
    //9. Print statistics-----------------------------
    
    stats_display();

    
    //10. Cleanup any allocated memory--------------------
    

    // destroy lock
    pthread_mutex_destroy(&mutex);
    sem_destroy(&fullLock);
    sem_destroy(&emptyLock);

    // Cleanup stats.

    stats_cleanup();

    return 0;
}

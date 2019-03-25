/*
 Rad Wadud
 CSCE 4600
 Project 1
 email: radwadud@my.unt.edu
 
 reference:
 https://docs.oracle.com/cd/E19455-01/806-5257/sync-12/index.html
 https://www.geeksforgeeks.org/use-posix-semaphores-c/
 http://www.cse.psu.edu/~deh25/cmpsc473/programs/semaphore_sample.c
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//mutex and semaphore declearation
pthread_mutex_t mutex_lock;
sem_t students_sem;
sem_t ta_sem;

#define MAX_SLEEP_TIME 5
#define NUM_OF_STUDENTS 5
#define NUM_OF_HELPS 3
#define NUM_OF_SEATS 3

int availableSeats[NUM_OF_SEATS];
int emptySeat,nextStudent;

int busySeat=0;

void init()
{
    sem_init(&ta_sem,1,0);
    sem_init(&students_sem,1,0);
    pthread_mutex_init(&mutex_lock,NULL);
    emptySeat = 0;
    nextStudent = 0;
}

void* create_ta(void *arg)
{
    int healpTime;
    printf("TA thread started\n");
    
    while(1)
    {
        sem_wait(&students_sem); //TA waiting
        healpTime = rand() % MAX_SLEEP_TIME + 1;
        
        pthread_mutex_lock(&mutex_lock);
        //prints who are waiting
        printf("\t%d students waiting\n",busySeat);
        
        printf("\tTA helping student# %d from seat #%d\n",availableSeats[nextStudent],(nextStudent+1));
        busySeat--;
        nextStudent = (nextStudent + 1)%NUM_OF_SEATS;
        
        pthread_mutex_unlock(&mutex_lock);
        sleep(healpTime);
        printf("\tTA is avaiable\n");
        sem_post(&ta_sem); //TA free
    }
    pthread_exit(NULL);
}

void *create_Student(void *thread_id)
{
    int studentID = (int)thread_id;
    int counter = 0;
    int helpingTime;
    printf("create student# %d\n",studentID);
    
    while(1)
    {
        printf("Student# %d working...\n",studentID);
        helpingTime= rand() % MAX_SLEEP_TIME + 1;
        sleep(helpingTime);
        printf("Student# %d going to TA for help #%d.\n",studentID,(counter));
        
        pthread_mutex_lock(&mutex_lock);
        if(busySeat == NUM_OF_SEATS) //Check if seats are occupied
        {
            printf("Seats NOT avaiable for student %d. Going back.\n",studentID);
            pthread_mutex_unlock(&mutex_lock);
            continue;
        }
        else
        {
            availableSeats[emptySeat]=studentID;
            busySeat++;
            printf("Student# %d waiting for TA in seat #%d\n",studentID,(emptySeat+1));
            emptySeat = (emptySeat + 1)%NUM_OF_SEATS;
            pthread_mutex_unlock(&mutex_lock);
            sem_post(&students_sem);//Wake TA for help
            
            pthread_mutex_unlock(&mutex_lock);
            
            sem_wait(&ta_sem); //wait for TA
            counter++;
            if(counter==NUM_OF_HELPS) //if maximum number of help
                break;
        }
    }
    printf("Student# %d has been helped.\n",studentID);
    pthread_exit(NULL);
}

int main()
{
    pthread_t taThread,studentThread[NUM_OF_STUDENTS];
    int i=0;
    init();
    
    //TA thread created
    pthread_create(&taThread,NULL,create_ta,NULL);
    
    //student thread created
    for(i = 0; i < NUM_OF_STUDENTS; i++)
    {
        pthread_create(&studentThread[i],NULL,create_Student,(void *)i);
    }
    
    for(i = 0; i < NUM_OF_STUDENTS; i++)
    {
        pthread_join(studentThread[i],NULL);
    }
    //finshed work
    printf("\nTA healped everyone. Program ending...\n");
    pthread_cancel(taThread);
}


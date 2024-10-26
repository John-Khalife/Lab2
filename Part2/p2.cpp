#include <iostream>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>

/**
 * This method decrements the semephore given
 * @param sem_id - the id of the semaphore to be decremented
*/
void reserveSemaphore(int sem_id) {
    struct sembuf toggler;
    toggler.sem_num = 0;
    toggler.sem_flg = 0;
    toggler.sem_op = -1;
    semop(sem_id,&toggler,1);
}

/**
 * This method increments the semaphore given.
 * @param sem_id - the id of the semaphore to be incremented
*/
void unReserveSemaphore(int sem_id) {
    struct sembuf toggler;
    toggler.sem_num = 0;
    toggler.sem_flg = 0;
    toggler.sem_op = 1;
    semop(sem_id,&toggler,1);
}

int main() {
    //Grab the shared memory belonging to this process.
    int shm_id = shmget(1234,sizeof(int), 0); 
    if (shm_id < 0) {
        perror("Unable to grab the shared memory in p2.");
        exit(1);
    }

    //attach the memory to process 2.
    int* generated_num = (int*) shmat(shm_id,NULL,0666); // Now generated_num is accesssible.

    //Grab the semaphore made in p1
    int sem_id = semget(1234,1,0666); //Note: middle parameter is the number of semaphores in the set.
    if (sem_id < 0) {
        perror("Getting the semaphore failed.");
        exit(1);
    }

    //Start an infinite loop - only ends when generated num (by process 1) is zero.
    for (int i = 0 ; i < 10 ; i++) {
        sleep(2);
        reserveSemaphore(sem_id);
        std::cout << "I am process 2. Generated num: " << (*generated_num) << std::endl;
        unReserveSemaphore(sem_id);
    }
    std::cout << "Ten repetitions finished. Exiting process 2." << std::endl;
    exit(0);
}
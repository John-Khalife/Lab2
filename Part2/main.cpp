#include <iostream>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>


/**
 * This method is intended to generate a random number between 0 and ten
 * It then assigns that number to the shared memory between processes.
 * @param generated_num - this is the pointer to where the generated number will be stored.
 * 
*/
void generateNumber(int* generated_num) {
    (*generated_num) = rand() % 11;
    if ((*generated_num) > 5) {
        std::cout << "High value" << std::endl;
    } else {
        std::cout << "Low value" << std::endl;
    }
}

void reserveSemaphore(int sem_id) {
    struct sembuf toggler;
    toggler.sem_num = 0;
    toggler.sem_flg = 0;
    toggler.sem_op = -1;
    semop(sem_id,&toggler,1);
}

void unReserveSemaphore(int sem_id) {
    struct sembuf toggler;
    toggler.sem_num = 0;
    toggler.sem_flg = 0;
    toggler.sem_op = 1;
    semop(sem_id,&toggler,1);
}

int main() { 
    std::cout << "Program start" << std::endl;
    srand(time(0)); //generate seed.
    //Make process 1.
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("There was an error creating the fork");
        exit(0);
    }

    //Create a shared memory segment.
    /*
    IPC_CREAT - create if not exists
    | 0666 read and write priviledges
    */
    int shm_id = shmget(1234,sizeof(int), IPC_CREAT | 0666); 
    if (shm_id < 0) {perror("failed shared memory allocation.");}

    //Attach the memory segment to process 1. It must store the generated number
    /*
    second parameter - where to attach the address. NULL means the system chooses.
    */
    int* generated_num = (int*) shmat(shm_id,NULL,0);

    //Create a semaphore.
    int sem_id = semget(1234,1,IPC_CREAT | 0666); //Note: middle parameter is the number of semaphores in the set.
    if (sem_id < 0) {
        perror("Creating a semaphore failed.");
        exit(1);
    }

    //Initialize semaphore
    if (semctl(sem_id, 0, SETVAL, 1) < 0) {
        perror("Initializing a semaphore failed.");
        exit(1);
    }

    if (pid1 == 0) {
        while (1) {
            sleep(2); //wait 2 seconds for ease of display
            reserveSemaphore(sem_id);
            generateNumber(generated_num); //generate the number
            std::cout << "I am process 1. Generated num: " << (*generated_num) << std::endl;
            int number = *generated_num;
            unReserveSemaphore(sem_id);

            //Check if the generated number is equal to nine
            if (number == 9) {
                pid_t pid2 = fork();
                if (pid2 < 0) {
                    std::cout << "Process 2 creation failed" << std::endl;
                    exit(0);
                } else if (pid2 == 0) {
                    execl("./p2","p2",NULL);
                }
                std::cout << "Process 2 pid: " << pid2 << std::endl;
                int status;
                pid_t child_pid = wait(&status); //wait on process 2.
                exit(0); //Exit process 1.
            } else if (number == 0) {
                //Finish if the generated number is zero.
                std::cout << "The generated number is zero, exiting process 1." << std::endl;
                exit(0);
            }
        }
    } else if (pid1 < 0) {
        std::cout << "Process 1 creation failed" << std::endl;
        exit(0);
    }
    std::cout << "Process 1 pid: " << pid1 << std::endl;
    exit(0);
}


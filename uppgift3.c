#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
  #include <sys/wait.h>
  #include "./helperfunctions.h"

#define SHMSIZE 128
const char *semName1 = "my_sema5";
const char *semName2 = "my_sema6";

int main(int argc, char** argv) {

    struct shm_struct {
        int start, slut;
		int buffert[10];
	};
    
	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	pid_t pid = -1;
	int var1 = 0, var2 = 0, shmid = -1;
	struct shmid_ds *shm_buf;
    int status;

	sem_t *sem_id1 = sem_open(semName1, O_CREAT, O_RDWR, 0);
	sem_t *sem_id2 = sem_open(semName2, O_CREAT, O_RDWR, 10);

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);
	pid = fork();
    
    srand(time(NULL));
	
    if (pid != 0) {
		/* here's the parent, acting as producer */
		while (var1 < 100) {
			/* write to shmem */
			var1++;
            sem_wait(sem_id2); /* busy wait until the buffer is empty */
			printf("Skickar: %d\n", var1); fflush(stdout);

            shmp->buffert[shmp->slut] = var1;
            shmp->slut = (shmp->slut + 1) % 10; // vänta på att sem_id2 är 1
            sem_post(sem_id1); // sätt att sem_id1 är 1
			sleepRandom(0.1f, 0.5f);
		}
        
        sem_close(sem_id1);
        sem_close(sem_id2);
        wait(&status);
        sem_unlink(semName1);
        sem_unlink(semName2);
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	} else {
		/* here's the child, acting as consumer */
		while (var2 < 100) {
			/* read from shmem */
            sem_wait(sem_id1);// vänta på sem_id1

            var2 = shmp->buffert[shmp->start];
            shmp->start = (shmp->start + 1) % 10;
            printf("Tar emot: %d\n", var2); fflush(stdout);
            sem_post(sem_id2);// sätt att sem_id2 är 1
			sleepRandom(0.2f, 2.0f);
		}
        sem_close(sem_id1);
        sem_close(sem_id2);
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	}
    
}
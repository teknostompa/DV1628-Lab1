#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>

#define SHMSIZE 128
int main(int argc, char** argv) {

    struct shm_struct {
        int start, slut;
		unsigned nummerIBuffert;
		int buffert[10];
	};
	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	pid_t pid = -1;
	int var1 = 0, var2 = 0, shmid = -1;
	struct shmid_ds *shm_buf;

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);
	shmp->nummerIBuffert = 0;
	pid = fork();
    
    srand(time(NULL));
	
    if (pid != 0) {
		/* here's the parent, acting as producer */
		while (var1 < 100) {
			/* write to shmem */
			var1++;
			while (shmp->nummerIBuffert >= 10); /* busy wait until the buffer is empty */
			printf("Skickar: %d\n", var1); fflush(stdout);

            shmp->buffert[shmp->slut] = var1;
            shmp->slut = (shmp->slut + 1) % 10;
            /* Båda programmen når denna storlek, vilket gör att det kan bli race condition. Detta gör att vi kan råka läsa fler nummer än vad som finns, eller att dataförlust uppstår.*/
			shmp->nummerIBuffert++;
            
            sleep((((float)rand()/(float)(RAND_MAX)) * 0.4f)+0.1);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	} else {
		/* here's the child, acting as consumer */
		while (var2 < 100) {
			/* read from shmem */
			while (shmp->nummerIBuffert == 0); /* busy wait until there is something */

            var2 = shmp->buffert[shmp->start];
            shmp->start = (shmp->start + 1) % 10;
            printf("Tar emot: %d\n", var2); fflush(stdout);
            /* Båda programmen når denna storlek, vilket gör att det kan bli race condition. Detta gör att vi kan råka läsa fler nummer än vad som finns, eller att dataförlust uppstår.*/
            shmp->nummerIBuffert--;
            sleep((((float)rand()/(float)(RAND_MAX)) * 1.8f)+0.2);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	}
}
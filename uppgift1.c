#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    pid_t process1, process2;
    unsigned processRaknare = 0;
    unsigned niterations = 100;
    unsigned processNummer;

    process1 = fork();
    processRaknare++;
    if(process1 != 0) {
        processRaknare++;
        process2 = fork();
    }

    processNummer = (process1 != 0 && process2 != 0)? 0 : processNummer;
    processNummer = (process1 == 0 && process2 != 0)? 1 : processNummer;
    processNummer = (process1 != 0 && process2 == 0)? 2 : processNummer;

    switch (processNummer)
    {
    case 0:

        for (int i = 0; i < niterations; i++){
            printf("B = %d, ", i);
        }
        
        for (int i = 0; i < processRaknare; i++) {
            printf("\nProcesser = %d", process1+i);
        }

        break;
    case 1:

        for (int i = 0; i < niterations; i++)
            printf("A = %d, ", i);

        break;
    case 2:

        for (int i = 0; i < niterations; i++)
            printf("C = %d, ", i);

        break;
    default:
        break;
    }

    printf("\n");
    
    return 0;
}
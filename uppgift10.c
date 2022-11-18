#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "helperfunctions.h"

pthread_mutex_t locks[5];

unsigned nProfessors;

void printProf(unsigned id, char* s){
    char idstring[200] = "%i: ";
    char endstring[] = "\n";
    strcat(idstring,s);
    strcat(idstring,endstring);
    printf(idstring,id);
}

void* professor(void* buf) {
    unsigned long childID = (unsigned long)buf;

    unsigned left = childID-1;
    unsigned right = childID % nProfessors;
    //Behaviour
    printProf(childID, "Started thinking");
    sleepRandom(1.0f, 5.0f);
    printProf(childID, "Thinking -> Pick up left");
    pthread_mutex_lock(&locks[left]);
    printProf(childID, "Pick up left -> Thinking");
    sleepRandom(2.0f, 8.0f);
    printProf(childID, "Thinking -> Pick up right");
    pthread_mutex_lock(&locks[right]);
    printProf(childID, "Pick up right -> Eating");
    sleepRandom(5.0f, 10.0f);
    printProf(childID, "Eating -> Put down both");
    pthread_mutex_unlock(&locks[left]);
    pthread_mutex_unlock(&locks[right]);
    printProf(childID, "Done");


    return NULL;
}

int main(int argc, char** argv) {
    srand(time(NULL));
    pthread_t *professors;
    unsigned id;
    if (argc > 1)
        nProfessors = atoi(argv[1]);
    professors = malloc( nProfessors * sizeof(pthread_t) );
    for (id = 1; id <= nProfessors; id++)
        pthread_create(&(professors[id-1]), NULL, professor, (void*)(__intptr_t)id);
    for (id = 1; id <= nProfessors; id++)
        pthread_join(professors[id-1], NULL);
    free(professors);
}
#include<unistd.h>
void sleepRandom(float min, float max){
    sleep((((float)rand()/(float)(RAND_MAX)) * (max-min))+min);
}
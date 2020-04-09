#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>

sem_t pingsem, pongsem;

int shared = 0;

void *
ping(void *arg) 
{
    int i1;
    for (i1 = 0; i1 < 10; i1++) {
        sem_wait(&pingsem);
        shared++;
        printf("ping - %i\n", shared);
        sem_post(&pongsem);
    }
}

void *
pong(void *arg) 
{
    int i2;
    for (i2 = 0; i2 < 10; i2++) {
        sem_wait(&pongsem);
        shared--;
        printf("pong - %i\n", shared);
        sem_post(&pingsem);
    }
}

int 
main(void) 
{
    sem_init(&pingsem, 0, 1);
    sem_init(&pongsem, 0, 0);
    pthread_t ping_thread, pong_thread; 
    pthread_create(&ping_thread, NULL, ping, NULL);
    pthread_create(&pong_thread, NULL, pong, NULL);
    pthread_join(ping_thread, NULL);
    pthread_join(pong_thread, NULL);
    return 0;
}
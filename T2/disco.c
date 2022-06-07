#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "disco.h"

void discoInit(void) {
    
}

void discoDestroy(void) {

}

// declare aca sus variables globales
int varon_ticket = 0, dama_ticket = 0;
int varon_display = 0, dama_display = 0;
char *varon_name = "";
char *dama_name = "";
int dama_waiting = 0, varon_waiting = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

char *dama(char *nom) {
    // espero a que me toque
    pthread_mutex_lock(&mutex);
    int my_ticket = dama_ticket++;
    while (my_ticket != dama_display) {
        pthread_cond_wait(&cond, &mutex);
    }
    // busco pareja
    dama_name = nom;
    dama_waiting = 1;
    while (varon_waiting == 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    // encontre pareja, asi que despierto a mi compañero
    char *pareja = varon_name;
    varon_waiting = 0;
    pthread_cond_broadcast(&cond);
    while (dama_waiting == 1) {
        pthread_cond_wait(&cond, &mutex);
    }
    // aumento el display y finalizo despertando al resto
    dama_display++;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    return pareja;
}

char *varon(char *nom) {
    // espero a que me toque
    pthread_mutex_lock(&mutex);
    int my_ticket = varon_ticket++;
    while (my_ticket != varon_display) {
        pthread_cond_wait(&cond, &mutex);
    }
    // busco pareja
    varon_name = nom;
    varon_waiting = 1;
    while (dama_waiting == 0) {
        pthread_cond_wait(&cond, &mutex);
    }
    // encontre pareja, asi que despierto a mi compañero
    char *pareja = dama_name;
    dama_waiting = 0;
    pthread_cond_broadcast(&cond);
    while (varon_waiting == 1) {
        pthread_cond_wait(&cond, &mutex);
    }
    varon_display++;
    // aumento el display y finalizo despertando al resto
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    return pareja;
}

#include <pthread.h>
#include <stdio.h>

#include "pss.h"
#include "h2o.h"

typedef struct {
    H2O *water;
    pthread_cond_t cond;
    void *element;
} Request;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
Queue *hq;
Queue *oq;

void initH2O(void) {
    hq = makeQueue();
    oq = makeQueue();
}

void endH2O(void) {
    destroyQueue(hq);
    destroyQueue(oq);
    pthread_mutex_destroy(&mutex);
}

H2O *combineOxy(Oxygen *o) {
    pthread_mutex_lock(&mutex);
    if (queueLength(hq)>=2) {
        // Obtenemos los hidrogenos
        Request *req1 = (Request *)get(hq);
        Request *req2 = (Request *)get(hq);
        // Creamos a los hidrogenos
        H2O *water = makeH2O((Hydrogen *)req1->element, (Hydrogen *)req2->element, o);
        // Despertamos a los hidrogenos
        req1->water = water;
        pthread_cond_signal(&req1->cond);
        req2->water = water;
        pthread_cond_signal(&req2->cond);
        pthread_mutex_unlock(&mutex);
        return water;
    }
    Request req = {NULL, PTHREAD_COND_INITIALIZER, (void *)o};
    put(oq, &req);
    while (req.water == NULL) {
        pthread_cond_wait(&req.cond, &mutex);
    }
    H2O *water = req.water;
    pthread_mutex_unlock(&mutex);
    return water;
}

H2O *combineHydro(Hydrogen *h) {
    pthread_mutex_lock(&mutex);
    // Revisamos si hay un oxigeno y un hidrogeno para formar la molecula
    if (!emptyQueue(oq) && !emptyQueue(hq)) {
        // Obtenemos el oxigeno y el hidrogeno
        Request *req1 = (Request *)get(oq);
        Request *req2 = (Request *)get(hq);
        H2O *water = makeH2O((Hydrogen *)(req2->element), h, (Oxygen *)(req1->element));
        // Despertamos al oxigeno
        req1->water = water;
        pthread_cond_signal(&req1->cond);
        // Despertamos al hidrogeno
        req2->water = water;
        pthread_cond_signal(&req2->cond);
        pthread_mutex_unlock(&mutex);
        return water;
    }
    // Si no hay suficientes elementos para crear la molecula, esperamos
    Request req = {NULL, PTHREAD_COND_INITIALIZER, (void *)h};
    put(hq, &req);
    while (req.water == NULL) {
        pthread_cond_wait(&req.cond, &mutex);
    }
    H2O *water = req.water;
    pthread_mutex_unlock(&mutex);
    return water;
}

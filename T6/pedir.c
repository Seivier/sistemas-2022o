#include <stdio.h>

#include "pss.h"
#include "spin-locks.h"
#include "pedir.h"

// Defina aca sus variables globales
int sl = OPEN;
int last_cat = -1;
Queue *q[2];

void iniciar() {
  q[0] = makeQueue();
  q[1] = makeQueue();
}

void terminar() {
  destroyQueue(q[0]);
  destroyQueue(q[1]);
}

void pedir(int cat) {
  spinLock(&sl);
  if(last_cat == -1) {
    last_cat = cat;
    spinUnlock(&sl);
    return;
  }
  int cond = CLOSED;
  put(q[cat], &cond);
  spinUnlock(&sl);
  spinLock(&cond);
}

void devolver() {
  spinLock(&sl);
  int cat = last_cat;
  last_cat = -1;
  int * cond;
  if (queueLength(q[!cat]) > 0) {
    cond = get(q[!cat]);
    last_cat = !cat;
    spinUnlock(cond);
  } else if (queueLength(q[cat]) > 0) {
    cond = get(q[cat]);
    last_cat = cat;
    spinUnlock(cond);
  }
  spinUnlock(&sl);
}

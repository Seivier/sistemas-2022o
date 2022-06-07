#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

static NthQueue *q[2];

static int adentro[2]= { 0, 0 };

void nth_iniPub(void) {
  q[0]= nth_makeQueue();
  q[1]= nth_makeQueue();
}

void nth_endPub(void) {
}

void nEntrar(int sexo) {
  int opuesto= !sexo;
  START_CRITICAL
  if (adentro[opuesto] || !nth_emptyQueue(q[opuesto])) {
    nThread t = nSelf();
    nth_putBack(q[sexo], t);
    suspend(WAIT_SEM);
    schedule();
  }
  else
    adentro[sexo]++;
  END_CRITICAL
}

void nSalir(int sexo) {
  int opuesto= !sexo;
  START_CRITICAL
  adentro[sexo]--;
  if (adentro[sexo]==0) {
    while (!nth_emptyQueue(q[opuesto])) {
      nThread t = nth_getFront(q[opuesto]);
      adentro[opuesto]++;
      setReady(t);
      schedule();
    }
  }
  END_CRITICAL
}

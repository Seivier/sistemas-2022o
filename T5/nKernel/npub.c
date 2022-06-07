#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

static NthQueue *q[2];

static int adentro[2]= { 0, 0 };

void nth_iniPub(void) {
  q[0]= nth_makeQueue();
  q[1]= nth_makeQueue();
}

void nth_endPub() {
}

void wakeUpPub(nThread th) {
  if(nth_queryThread(q[0], th)) {
    nth_delQueue(q[0], th);
  } else {
    nth_delQueue(q[1], th);
  }
}

int nEntrarTimeout(int sexo, long long delayNanos) {
  if (delayNanos <= 0) 
    return 1;
  int opuesto= !sexo;
  int rc = 1;
  START_CRITICAL
  if (adentro[opuesto] || !nth_emptyQueue(q[opuesto])) {
    nThread t = nSelf();
    t->send.rc = rc;
    nth_programTimer(delayNanos, wakeUpPub);
    nth_putBack(q[sexo], t);
    suspend(WAIT_PUB_TIMEOUT);
    schedule();
    rc = t->send.rc;
  }
  else
    adentro[sexo]++;
  END_CRITICAL
  return rc;
}

void nEntrar(int sexo) {
  int opuesto= !sexo;
  START_CRITICAL
  if (adentro[opuesto] || !nth_emptyQueue(q[opuesto])) {
    nThread t = nSelf();
    nth_putBack(q[sexo], t);
    suspend(WAIT_PUB);
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
      if(t->status == WAIT_PUB_TIMEOUT) {
        t->send.rc = 0;
        nth_cancelThread(t);
      }
      setReady(t);
      schedule();
    }
  }
  END_CRITICAL
}

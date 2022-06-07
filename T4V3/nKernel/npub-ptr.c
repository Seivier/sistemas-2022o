// Esta solucion funciona pero no sirve para la tarea 4!
// Esta basada en los monitores y condiciones de nSystem que
// son basicamente mutex y condiciones.
// Tiene que reprogramar este codigo para basarse unicamente
// en las funciones de bajo nivel de nThreads para programar
// herramientas de sincronizacion.  Basese en la implementacion
// de los semaforos, mutex/condiciones y mensajes.


#define _XOPEN_SOURCE 500

#include <nSystem.h>
#include <fifoqueues.h>

#define entrar nEntrar
#define salir nSalir

typedef struct {
  int listo;
  nCondition cond;
} Request;

static nMonitor c;
static FifoQueue q[2];

static int adentro[2]= { 0, 0 };

void nth_iniPub(void) {
  c= nMakeMonitor();
  q[0]= MakeFifoQueue();
  q[1]= MakeFifoQueue();
}

void nth_endPub(void) {
}

void entrar(int sexo) {
  int opuesto= !sexo;
  nEnter(c);
  if (adentro[opuesto] || !EmptyFifoQueue(q[opuesto])) {
    Request req= { FALSE, nMakeCondition(c) };
    PutObj(q[sexo], &req);
    while (!req.listo)
      nWaitCondition(req.cond);
    nDestroyCondition(req.cond);
  }
  else
    adentro[sexo]++;
  nExit(c);
}

void salir(int sexo) {
  int opuesto= !sexo;
  nEnter(c);
  adentro[sexo]--;
  if (adentro[sexo]==0) {
    while (!EmptyFifoQueue(q[opuesto])) {
      Request *preq= GetObj(q[opuesto]);
      preq->listo= TRUE;
      nSignalCondition(preq->cond);
      adentro[opuesto]++;
    }
  }
  nExit(c);
}

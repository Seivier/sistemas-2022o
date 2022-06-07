#define _XOPEN_SOURCE 500

#include "nthread.h"

#define NMSGS 1000

typedef struct {
  void *msgRef, *msgRec;
  nThread thSend, thRef;
  long long timeout;
  int cnt, rc;
  long long trec;
} Args;

void *receptor(void *ptr) {
  Args *pa= ptr;
  for (int i= 0; i<pa->cnt; i++) {
    pa->msgRec= nReceiveNanos(&pa->thSend, pa->timeout);
    pa->trec= nGetTimeNanos();
    if (pa->msgRef!=NULL && pa->msgRef!=pa->msgRec)
      nFatalError("receptor", "mensaje incorrecto\n");
    if (pa->thRef!=NULL && pa->thRef!=pa->thSend)
      nFatalError("receptor", "emisor incorrecto\n");
    if (pa->msgRec!=NULL)
      nReply(pa->thSend, pa->rc);
  }
  return NULL;
}

void testSimple() {
  pthread_t th;
  int msg;
  printf("Probando %d mensajes sin timout\n", NMSGS);
  Args args= { &msg, NULL, NULL, nSelf(), -1, 1000, 34 };
  pthread_create(&th, NULL, receptor, &args);
  for (int i= 0; i<args.cnt; i++) {
    int rc= nSend(th, &msg);
    if (rc!=args.rc)
      nFatalError("testSimple", "Codigo de retorno de nSend incorrecto\n");
  }
  pthread_join(th, NULL);
  printf("Test aprobado\n");
}

// 1 second resolution
#define TICK 1000000000LL
#define PLAZO 200000000LL

void testTimeouts() {
  pthread_t ta, tb, tc, td;
  int msgA, msgB, msgC, msgD;
  Args a= { NULL, NULL, NULL, NULL, 3*TICK, 1, 23, 0 };
  Args b= { &msgB, NULL, NULL, nSelf(), 2*TICK, 1, 78, 0 };
  Args c= { &msgC, NULL, NULL, nSelf(), 4*TICK, 1, 91, 0 };
  Args d= { NULL, NULL, NULL, NULL, 1*TICK, 1, 35, 0 };
  
  pthread_create(&ta, NULL, receptor, &a);
  pthread_create(&tb, NULL, receptor, &b);
  pthread_create(&tc, NULL, receptor, &c);
  pthread_create(&td, NULL, receptor, &d);
  long long ini= nGetTimeNanos();
  pthread_join(td, NULL); // Timeout for a after 1 tick
  if (d.trec-ini<1*TICK)
    nFatalError("testTimeouts", "Thread d termina antes de tiempo\n");
  if (d.trec-ini>1*TICK+PLAZO)
    nFatalError("testTimeouts", "Thread d termina tarde\n");
  long long tmsgb= nGetTimeNanos();
  nSend(tb, &msgB);       // message reception at tEnd
  pthread_join(tb, NULL);
  if (b.trec<tmsgb)
    nFatalError("testTimeouts", "Thread d termina antes de tiempo\n");
  if (b.trec>tmsgb+PLAZO) // 200 millis de plazo para recibir mensaje
    nFatalError("testTimeouts", "Thread d termina tarde\n");
  pthread_join(ta, NULL); // Timeout for a after 3 tick
  if (a.trec-ini<3*TICK)
    nFatalError("testTimeouts", "Thread d termina antes de tiempo\n");
  if (a.trec>3*TICK+PLAZO)
    nFatalError("testTimeouts", "Thread d termina tarde\n");
  long long tmsgc= nGetTimeNanos();
  nSend(tc, &msgC);
    pthread_join(tc, NULL);
  if (c.trec<tmsgc)
    nFatalError("testTimeouts", "Thread d termina antes de tiempo\n");
  if (c.trec>tmsgc+PLAZO) // 200 millis de plazo para recibir mensaje
    nFatalError("testTimeouts", "Thread d termina tarde\n");
  printf("Test aprobado\n");
}
  
  
  
int nMain() {
  testSimple();
  testTimeouts();
  printf("Felicitades: todos los tests aprobados\n");
  return 0;
}
  

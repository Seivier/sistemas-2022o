#define _DEFAULT_SOURCE 1

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdarg.h>

#include "pss.h"
#include "h2o.h"

#define PAUSE 300000

#ifdef OPT
#define NOXY 50
#define NATOM 1000000
#else
#define NOXY 20
#define NATOM 100000
#endif

// Ver el nMain al final

void fatalError(char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vfprintf(stdout, format, ap);
  va_end(ap);
  fflush(stdout);
  exit(1);
}


// typedef struct h2o H2O;
struct h2o {
  Hydrogen *h1, *h2;
  Oxygen *o;
};

// typedef struct hydrogen Hydrogen;
struct hydrogen {
  unsigned i;
};

// typedef struct Oxygen Oxygen;
struct oxygen {
  unsigned long l;
};

typedef struct {
  pthread_mutex_t m;
  int cnt;
  int fill[64];
} Ctrl;

typedef struct {
  Ctrl *pctl;
  unsigned crcH;
} Args;

static void *oxygen(void *ptrO) {
  H2O *agua= combineOxy(ptrO);
  return agua;
}

static void *hydrogen(void *ptrH) {
  H2O *agua= combineHydro(ptrH);
  return agua;
}

H2O *makeH2O(Hydrogen *h1, Hydrogen *h2, Oxygen *o) {
  H2O *agua= malloc(sizeof(*agua));
  agua->h1= h1;
  agua->h2= h2;
  agua->o= o;
  return agua;
}

static void *oxyProd(void *ptr) {
  Args *pa= ptr;
  pa->crcH= 0;
  for (;;) {
    pthread_mutex_lock(&pa->pctl->m);
    if (pa->pctl->cnt==0) {
      pthread_mutex_unlock(&pa->pctl->m);
      break;
    }
    pa->pctl->cnt--;
    pthread_mutex_unlock(&pa->pctl->m);
    Oxygen *o= malloc(sizeof(*o));
    o->l= random();
    H2O *agua= combineOxy(o);
    pa->crcH += agua->h1->i;
    pa->crcH += agua->h2->i;
    free(o); free(agua->h1); free(agua->h2);
    free(agua);
  }
  return NULL;
}

static void *hydroProd(void *ptr) {
  Args *pa= ptr;
  pa->crcH= 0;
  for(;;) {
    pthread_mutex_lock(&pa->pctl->m);
    if (pa->pctl->cnt==0) {
      pthread_mutex_unlock(&pa->pctl->m);
      break;
    }
    pa->pctl->cnt--;
    pthread_mutex_unlock(&pa->pctl->m);
    Hydrogen *h= malloc(sizeof(*h));
    h->i= random();
    pa->crcH = pa->crcH + h->i;
    combineHydro(h);
  }
  return NULL;
}

/****************************************************
 * Programa principal
 ****************************************************/

int main(int argc, char **argv) {
  initH2O();
  printf("Primer test de prueba: semantica\n");
  {
    Oxygen o1, o2;
    Hydrogen h1, h2, h3, h4;
    pthread_t H1, H2, H3, H4, O1, O2;
    void *v1, *v2, *v3;
    pthread_create(&H1, NULL, hydrogen, &h1);
    usleep(PAUSE);
    pthread_create(&O1, NULL, oxygen, &o1);
    usleep(PAUSE);
    pthread_create(&O2, NULL, oxygen, &o2);
    usleep(PAUSE);
    pthread_create(&H2, NULL, hydrogen, &h2);
    pthread_join(H1, &v1);
    pthread_join(O1, &v2);
    pthread_join(H2, &v3);
    H2O *agua= v1;
    if (agua->h1 != &h1 || agua->h2 != &h2 || agua->o != &o1)
      fatalError("primera molecula mal formada\n");
    if (agua!=v2 || agua!=v3)
      fatalError("no retorna la misma 1era. molecula en todas las llamadas\n");
    free(agua);
    usleep(PAUSE);
    pthread_create(&H3, NULL, hydrogen, &h3);
    usleep(PAUSE);
    pthread_create(&H4, NULL, hydrogen, &h4);
    pthread_join(H3, &v1);
    pthread_join(H4, &v2);
    pthread_join(O2, &v3);
    agua= v1;
    if (agua->h1 != &h3 || agua->h2 != &h4 || agua->o != &o2)
      fatalError("segunda molecula mal formada\n");
    if (agua!=v2 || agua!=v3)
      fatalError("no retorna la misma 2da. molecula en todas las llamadas\n");
    free(agua);
    printf("Test aprobado\n");
  }

  printf("Test de robustez\n");

  Ctrl hctl, octl;
  octl.cnt= NATOM;
  hctl.cnt= 2*NATOM;
  pthread_mutex_init(&hctl.m, 0);
  pthread_mutex_init(&octl.m, 0);

  Args harg= {&hctl, 0}, oarg= {&octl, 0};
  pthread_t oxy_th[NOXY], hydro_th[2*NOXY];
  Args      oxy_a[NOXY], hydro_a[2*NOXY];
  unsigned crcH= 0, crcO=0;

  for (int k=0; k<NOXY; k++) {
    oxy_a[k]= oarg;
    hydro_a[2*k]= harg;
    hydro_a[2*k+1]= harg;
    pthread_create(&oxy_th[k], NULL, oxyProd, &oxy_a[k]);
    pthread_create(&hydro_th[2*k], NULL, hydroProd, &hydro_a[2*k]);
    pthread_create(&hydro_th[2*k+1], NULL, hydroProd, &hydro_a[2*k+1]);
  }

  for (int k=0; k<NOXY; k++) {
    pthread_join(oxy_th[k], NULL);
    crcO += oxy_a[k].crcH;
    pthread_join(hydro_th[2*k], NULL);
    pthread_join(hydro_th[2*k+1], NULL);
    crcH += hydro_a[2*k].crcH + hydro_a[2*k+1].crcH;
  }

  if (crcH!=crcO) {
    fprintf(stderr, "La suma de los hidrogenos no coincide con la calculada "
            "al combinar con oxigenos\n");
    exit(1);
  }

  printf("Felicitaciones: todos los tests aprobados\n");

  endH2O();
  return 0;
}


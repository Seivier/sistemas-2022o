#define _DEFAULT_SOURCE 1

#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "sat.h"

#ifdef OPT
#define N_INTENTOS 5
#define TOLERANCIA 1.5
#else
#define N_INTENTOS 1
#define TOLERANCIA 1.0
#endif

#define RECUERDO "\nEste test puede fallar con una solucion correcta si\n" \
  "su computadora esta ejecutando otros procesos intensivos en CPU\n" \
  "Intente terminar procesos que puedan gastar CPU como por ejemplo el\n" \
  "navegador y coloque su notebook en modo alto rendimiento.  Si con esto\n" \
  "no se resuelve el problema, es porque su solucion es incorrecta.\n"

// ----------------------------------------------------
// Funcion que entrega el tiempo transcurrido desde el lanzamiento del
// programa en milisegundos

static int time0= 0;

static int getTime0() {
    struct timeval Timeval;
    gettimeofday(&Timeval, NULL);
    return Timeval.tv_sec*1000+Timeval.tv_usec/1000;
}

void resetTime() {
  time0= getTime0();
}

int getTime() {
  return getTime0()-time0;
}

// ----------------------------------------------------
// La funcion f4 para la version secuencial
//

static int f4(int x[]) {
  return (x[0]||!x[1])&&(!x[1]||x[2]||!x[3]);
}

// ----------------------------------------------------
// La funcion f4 para la version paralela
// Verifica el numero de threads usado
//

#define MAXT 64

static int tcnt= 0, tlim= 1;
static pthread_t tset[MAXT];
static pthread_mutex_t m= PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t c= PTHREAD_COND_INITIALIZER;

static int f4b(int x[]) {
  pthread_t self= pthread_self();
  pthread_mutex_lock(&m);

  int i= 0;
  while (i<tcnt) {
    if (self==tset[i])
      break;
    i++;
  }

  if (i==tcnt) {
    tset[tcnt]= self;
    tcnt++;
    pthread_cond_broadcast(&c);

    int end= getTime()+3000;
    while (tcnt<tlim) {
      int remain= end-getTime();
      if (remain<=0)
        break;
      struct timespec delay= { remain/1000, (remain%1000)*1000000};
      pthread_cond_timedwait(&c, &m, &delay);
    }
    if (tcnt<tlim) {
      fprintf(stderr, "\n***Error: se usaron solo %d thread ***\n", tcnt);
      exit(1);
    }
    else if (tcnt>tlim) {
      fprintf(stderr, "\n*** Error: se usaron mas threads que los "
                      "especificados ***\n");
      exit(1);
    }
  }
  pthread_mutex_unlock(&m);
  return (x[0]||!x[1])&&(!x[1]||x[2]||!x[3]);
}

static int f_big(int x[]) {
  return (x[0]&&!x[1]&&x[5]) || (!x[1]&&x[2]&&!x[3]&&x[6]) ||
         (x[2]&&x[6]&&!x[7]&&x[8]) || (!x[5]&&x[9]&&x[0]) ||
         (x[3]&&x[7]&&x[10]&&!x[1]) || (x[6]&&!x[8]&&x[11]) ||
         (x[5]&&x[12]&&!x[13]&&x[2]) || (x[12]&&!x[14]&&!x[15]) ||
         (!x[13]&&x[9]&&x[16]&&!x[17]) || (x[15]&&!x[18]&&x[19]) ||
         (x[14]&&x[18]&&x[20]&&!x[21]) || (x[15]&&x[20]&&!x[22]) ||
         (x[20]&&x[21]&&!x[19]&&x[23]) || (x[15]&&!x[22]&&x[24]) ||
         (x[22]&&x[23]&&!x[14]&&x[25]) || (x[25]&&!x[18]&&x[26]) ||
         (x[23]&&!x[24]&&!x[13]&&x[27]) || (x[21]&&!x[16]&&!x[28]);
}

static int cnt= 0;

static void gen(int x[], int i, int n, BoolFun f) {
  if (i==n) {
    if ((*f)(x))
      cnt++;
  }
  else {
    x[i]= 0; gen(x, i+1, n, f);
    x[i]= 1; gen(x, i+1, n, f);
} }

static int recuentoSec(int n, BoolFun f) {
  int x[n];
  cnt= 0;
  gen(x, 0, n, f);
  return cnt;
}

static int n_true_f_big_sec;
static int tiempo_sec;

static double compare_f_big() {
  printf("Calculando recuento paralelo para f_big usando 2 threads\n");
  resetTime();
  int n_true_f_big= recuento(29, f_big, 1);
  int tiempo_par= getTime();
  printf("recuento paralelo para f_big: %d (%d milisegs.)\n",
         n_true_f_big, tiempo_par);
  if (n_true_f_big!=n_true_f_big_sec) {
    fprintf(stderr, "recuento incorrecto para f_big: %d!=%d\n",
            n_true_f_big, n_true_f_big_sec);
    exit(1);
  }
  double speedUp= (double)tiempo_sec / tiempo_par;
  return speedUp;
}

int main() {

  printf("Calculando recuento secuencial de f4\n");
  int n_true_f4_sec= recuentoSec(4, f4);
  for (int p= 0; p<5; p++) {
    printf("Calculando recuento paralelo de f4 usando %d threads\n", 1<<p);
    tcnt= 0; tlim= 1<<p;
    int n_true_f4= recuento(4, f4b, p);
    printf("Threads usados: %d\n", tcnt);
    if (n_true_f4!=n_true_f4_sec) {
      fprintf(stderr, "recuento incorrecto para f4: %d!=%d\n",
              n_true_f4, n_true_f4_sec);
      exit(1);
    }
  }
  printf("Recuento paralelo de f4 es correcto\n");

  printf("\nCalculando recuento secuencial para f_big\n");
  printf("~6 segundos en mi PC (Debian 11 Mate, Ryzen 5 3550H)\n");
  printf("Debe probar su tarea en una maquina desocupada "
         "con al menos 2 cores\n");
  resetTime();
  n_true_f_big_sec= recuentoSec(29, f_big);
  tiempo_sec= getTime();
  printf("recuento secuencial para f_big: %d (%d milisegs.)\n",
         n_true_f_big_sec, tiempo_sec);
  int intentos= 0;
  double speedUp;
  do {
    intentos++;
    printf("Intento %d de %d\n", intentos, N_INTENTOS);
    speedUp= compare_f_big();
    if (speedUp<TOLERANCIA)
      fprintf(stderr, "Factor de mejora (speed up) es insuficiente: %f\n",
              speedUp);
    else
      printf("Factor de mejora aprobado: %f\n", speedUp);
  } while (intentos<N_INTENTOS && speedUp<TOLERANCIA);

  if (speedUp<TOLERANCIA) {
    fprintf(stderr, "\n*** El factor de mejora debe ser al menos %f ***\n",
                     TOLERANCIA);
    fprintf(stderr, RECUERDO);
    exit(1);
  }
 
  printf("\n\nFelicitaciones: su tarea satisface el enunciado de la tarea\n");

  return 0;
}

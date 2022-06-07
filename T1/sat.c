#include <pthread.h>
#include <stdio.h>
#include "sat.h"

typedef struct
{
  int* z;
  int i;
  int p;
  int n;
  BoolFun f;
  int cnt;
} Args;

static int gen(int z[], int i, int p, int n, BoolFun f);

void copyInt(int* src, int* dest, int n) {
  for (int i = 0; i < n; i++) {
    dest[i] = src[i];
  }
}

static int genSec(int x[], int i, int n, BoolFun f) {
  if (i==n) {
    if ((*f)(x))
      return 1;
    return 0;
  }
  else {
    x[i]= 0; 
    int c1 = genSec(x, i+1, n, f);
    x[i]= 1; 
    int c2 = genSec(x, i+1, n, f);
    return c1 + c2;
} }

static int recuentoSec(int x[], int i, int n, BoolFun f) {
  return genSec(x, i, n, f);
}

void *thread(void* p) {
  Args *args = (Args*) p;
  args->cnt = gen(args->z, args->i, args->p, args->n, args->f);
  return NULL;
}

int gen(int z[], int i, int p, int n, BoolFun f) {
  if (i == p) {
    return recuentoSec(z, i, n, f);
  }
  z[i] = 0; 
  pthread_t pid;
  int x[n];
  copyInt(z, x, p);
  Args args = {x, i+1, p, n, f, 0};
  pthread_create(&pid, NULL, thread, &args);
  z[i] = 1; 
  int cnt = gen(z, i+1, p, n, f);
  pthread_join(pid, NULL);
  return cnt + args.cnt;
}

int recuento(int n, BoolFun f, int p) {
  int z[n];
  return gen(z, 0, p, n, f);
}

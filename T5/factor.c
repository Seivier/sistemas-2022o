// Determina usando P cores un factor de un entero
// Numero grande no primo: 229442531844556801   primo: 2305843009213693951

#define _XOPEN_SOURCE 500

#include "nthread.h"
#include <math.h>

typedef unsigned long long ulonglong;
typedef unsigned int uint;

// busca un factor del numero entero x en el rango [i, j]
uint buscarFactor(ulonglong x, uint i, uint j) {
    printf("Buscando en [%u,%u]\n", i, j);
    for (uint k=i; k<=j; k++) {
        if (x%k == 0)
            return k; 
    }
    return 0;
}

typedef struct {
    ulonglong x;
    uint i;
    uint j;
    uint res;
} Args;

void *thread(void* ptr) {
    Args *args = (Args*) ptr;
    ulonglong x = args->x;
    uint i = args->i;
    uint j = args->j;
    
    args->res = buscarFactor(x, i, j);
    return NULL;
}

// busca un factor del número entero x en el rango [i, j]
// utilizando P cores
uint buscarFactorParalelo(ulonglong x, uint i, uint j, int p) {
    pthread_t pid[p];
    Args args[p];    
    
    uint intervalo = (j-i+p)/p;
    printf("intervalo= %u\n", intervalo);
    for (int k=0; k<p; k++) {
        args[k].x = x;
        args[k].i = i + intervalo*k;
        args[k].j = i + intervalo*(k+1) -1;
        if (k==p-1)
          args[k].j= j;
        
        pthread_create(&pid[k], NULL, thread, &args[k]);
    }
    
    uint factor = 0;
    for (int k=0; k<p; k++) {
        pthread_join(pid[k], NULL);
        if (args[k].res != 0)
            factor = args[k].res;
    }   
    return factor;
}

int main(int argc, char *argv[]) {
  if (argc!=3) {
    fprintf(stderr, "uso: %s <num> <nthreads>\n", argv[0]);
    exit(1);
  }
  long long x= atoll(argv[1]);
  int p= atoi(argv[2]);
  uint rx= sqrt(x);
  while ((long long)rx*rx<x)
    rx++;
  uint f= buscarFactorParalelo(x, 2, rx, p);
  printf("factor de %lld = %u\n", x, f);
  return 0;
}

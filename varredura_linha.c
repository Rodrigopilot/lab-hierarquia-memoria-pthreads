#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double calcular_tempo(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) +
           (fim.tv_nsec - inicio.tv_nsec) / 1000000000.0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <tamanho_da_matriz>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);

    if (N <= 0) {
        printf("O tamanho da matriz deve ser maior que zero.\n");
        return 1;
    }

    size_t total_elementos = (size_t)N * N;

    double *A = malloc(total_elementos * sizeof(double));

    if (A == NULL) {
        printf("Erro ao alocar memoria para a matriz.\n");
        return 1;
    }

    /* Inicializacao padronizada da matriz */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[(size_t)i * N + j] = (double)(i + j);
        }
    }

    long long pares = 0;
    struct timespec inicio;
    struct timespec fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    /* Varredura por linha: i externo e j interno */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if ((long long)A[(size_t)i * N + j] % 2 == 0) {
                pares++;
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo = calcular_tempo(inicio, fim);

    printf("Varredura por linha\n");
    printf("N: %d\n", N);
    printf("Elementos pares: %lld\n", pares);
    printf("Tempo: %.9f segundos\n", tempo);

    free(A);

    return 0;
}
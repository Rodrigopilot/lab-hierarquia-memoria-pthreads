#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double calcular_tempo(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) +
           (fim.tv_nsec - inicio.tv_nsec) / 1000000000.0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <tamanho_da_matriz>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    if (n <= 0) {
        printf("O tamanho deve ser maior que zero.\n");
        return 1;
    }

    size_t total = (size_t)n * (size_t)n;
    double *matriz = malloc(total * sizeof(double));

    if (matriz == NULL) {
        printf("Erro ao alocar memoria.\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matriz[(size_t)i * n + j] = (double)(i + j);
        }
    }

    long long pares = 0;
    struct timespec inicio;
    struct timespec fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if ((long long)matriz[(size_t)i * n + j] % 2 == 0) {
                pares++;
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);

    printf("Varredura por linha\n");
    printf("N: %d\n", n);
    printf("Elementos pares: %lld\n", pares);
    printf("Tempo: %.9f segundos\n", calcular_tempo(inicio, fim));

    free(matriz);
    return 0;
}
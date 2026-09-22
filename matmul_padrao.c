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
        printf("Uso: %s <N>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    if (n <= 0) {
        printf("N deve ser maior que zero.\n");
        return 1;
    }

    size_t total = (size_t)n * n;

    double *A = malloc(total * sizeof(double));
    double *B = malloc(total * sizeof(double));
    double *C = calloc(total, sizeof(double));

    if (A == NULL || B == NULL || C == NULL) {
        printf("Erro ao alocar memoria.\n");
        free(A);
        free(B);
        free(C);
        return 1;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[(size_t)i * n + j] = (double)(i + j);
            B[(size_t)i * n + j] = (double)(i * j);
        }
    }

    struct timespec inicio;
    struct timespec fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double soma = 0.0;

            for (int k = 0; k < n; k++) {
                soma += A[(size_t)i * n + k] *
                        B[(size_t)k * n + j];
            }

            C[(size_t)i * n + j] = soma;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo = calcular_tempo(inicio, fim);
    double operacoes = 2.0 * n * n * n;
    double gflops = operacoes / (tempo * 1000000000.0);
    double checksum = 0.0;

    for (size_t i = 0; i < total; i++) {
        checksum += C[i];
    }

    printf(
        "[Matmul Padrao] N: %d | Tempo: %.6f s | "
        "GFLOPS: %.3f | Checksum: %.6e\n",
        n, tempo, gflops, checksum
    );

    free(A);
    free(B);
    free(C);

    return 0;
}
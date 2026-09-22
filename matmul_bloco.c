#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double calcular_tempo(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) +
           (fim.tv_nsec - inicio.tv_nsec) / 1000000000.0;
}

static int minimo(int a, int b) {
    return a < b ? a : b;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <N> <tamanho_bloco>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int bloco = atoi(argv[2]);

    if (n <= 0 || bloco <= 0) {
        printf("N e tamanho do bloco devem ser maiores que zero.\n");
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

    for (int ii = 0; ii < n; ii += bloco) {
        for (int jj = 0; jj < n; jj += bloco) {
            for (int kk = 0; kk < n; kk += bloco) {
                int limite_i = minimo(ii + bloco, n);
                int limite_j = minimo(jj + bloco, n);
                int limite_k = minimo(kk + bloco, n);

                for (int i = ii; i < limite_i; i++) {
                    for (int k = kk; k < limite_k; k++) {
                        double r = A[(size_t)i * n + k];

                        for (int j = jj; j < limite_j; j++) {
                            C[(size_t)i * n + j] +=
                                r * B[(size_t)k * n + j];
                        }
                    }
                }
            }
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
        "[Matmul Bloco] N: %d | Bloco: %d | Tempo: %.6f s | "
        "GFLOPS: %.3f | Checksum: %.6e\n",
        n, bloco, tempo, gflops, checksum
    );

    free(A);
    free(B);
    free(C);

    return 0;
}
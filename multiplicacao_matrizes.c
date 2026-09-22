#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static double calcular_tempo(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) +
           (fim.tv_nsec - inicio.tv_nsec) / 1000000000.0;
}

static int minimo(int a, int b) {
    return a < b ? a : b;
}

static void inicializar_matrizes(double *A, double *B, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[(size_t)i * n + j] = (double)((i + j) % 100) / 100.0;
            B[(size_t)i * n + j] = (double)((2 * i + j) % 100) / 100.0;
        }
    }
}

static void multiplicacao_tradicional(
    const double *A,
    const double *B,
    double *C,
    int n
) {
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
}

static void multiplicacao_blocos(
    const double *A,
    const double *B,
    double *C,
    int n,
    int bloco
) {
    for (int ii = 0; ii < n; ii += bloco) {
        for (int jj = 0; jj < n; jj += bloco) {
            for (int kk = 0; kk < n; kk += bloco) {
                int limite_i = minimo(ii + bloco, n);
                int limite_j = minimo(jj + bloco, n);
                int limite_k = minimo(kk + bloco, n);

                for (int i = ii; i < limite_i; i++) {
                    for (int j = jj; j < limite_j; j++) {
                        double soma = C[(size_t)i * n + j];

                        for (int k = kk; k < limite_k; k++) {
                            soma += A[(size_t)i * n + k] *
                                    B[(size_t)k * n + j];
                        }

                        C[(size_t)i * n + j] = soma;
                    }
                }
            }
        }
    }
}

static double calcular_checksum(const double *C, int n) {
    double soma = 0.0;
    size_t total = (size_t)n * n;

    for (size_t i = 0; i < total; i++) {
        soma += C[i];
    }

    return soma;
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        printf(
            "Uso: %s <N> <tradicional|blocos> [tamanho_bloco]\n",
            argv[0]
        );
        return 1;
    }

    int n = atoi(argv[1]);
    int bloco = 32;

    if (argc == 4) {
        bloco = atoi(argv[3]);
    }

    if (n <= 0 || bloco <= 0) {
        printf("N e tamanho do bloco devem ser maiores que zero.\n");
        return 1;
    }

    size_t total = (size_t)n * n;

    double *A = malloc(total * sizeof(double));
    double *B = malloc(total * sizeof(double));
    double *C = calloc(total, sizeof(double));

    if (A == NULL || B == NULL || C == NULL) {
        printf("Erro ao alocar memoria para as matrizes.\n");
        free(A);
        free(B);
        free(C);
        return 1;
    }

    inicializar_matrizes(A, B, n);

    struct timespec inicio;
    struct timespec fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    if (strcmp(argv[2], "tradicional") == 0) {
        multiplicacao_tradicional(A, B, C, n);
    } else if (strcmp(argv[2], "blocos") == 0) {
        multiplicacao_blocos(A, B, C, n, bloco);
    } else {
        printf("Metodo invalido. Use tradicional ou blocos.\n");
        free(A);
        free(B);
        free(C);
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);

    printf("Multiplicacao de matrizes\n");
    printf("N: %d\n", n);
    printf("Metodo: %s\n", argv[2]);

    if (strcmp(argv[2], "blocos") == 0) {
        printf("Tamanho do bloco: %d\n", bloco);
    }

    printf("Tempo: %.9f segundos\n", calcular_tempo(inicio, fim));
    printf("Checksum: %.6f\n", calcular_checksum(C, n));

    free(A);
    free(B);
    free(C);

    return 0;
}
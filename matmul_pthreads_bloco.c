#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int id;
    int n;
    int bloco;
    int numero_threads;
    double *A;
    double *B;
    double *C;
} DadosThread;

double calcular_tempo(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) +
           (fim.tv_nsec - inicio.tv_nsec) / 1000000000.0;
}

int menor(int a, int b) {
    return a < b ? a : b;
}

void *multiplicar_blocos(void *arg) {
    DadosThread *dados = (DadosThread *)arg;

    int n = dados->n;
    int bloco = dados->bloco;

    int linha_inicial =
        dados->id * n / dados->numero_threads;

    int linha_final =
        (dados->id + 1) * n / dados->numero_threads;

    for (int ii = linha_inicial; ii < linha_final; ii += bloco) {
        for (int jj = 0; jj < n; jj += bloco) {
            for (int kk = 0; kk < n; kk += bloco) {

                int limite_i = menor(ii + bloco, linha_final);
                int limite_j = menor(jj + bloco, n);
                int limite_k = menor(kk + bloco, n);

                for (int i = ii; i < limite_i; i++) {
                    for (int k = kk; k < limite_k; k++) {
                        double valor_a =
                            dados->A[(size_t)i * n + k];

                        for (int j = jj; j < limite_j; j++) {
                            dados->C[(size_t)i * n + j] +=
                                valor_a *
                                dados->B[(size_t)k * n + j];
                        }
                    }
                }
            }
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf(
            "Uso: %s <tamanho_da_matriz> "
            "<numero_de_threads> <tamanho_do_bloco>\n",
            argv[0]
        );
        return 1;
    }

    int n = atoi(argv[1]);
    int numero_threads = atoi(argv[2]);
    int bloco = atoi(argv[3]);

    if (n <= 0 || numero_threads <= 0 || bloco <= 0) {
        printf(
            "O tamanho da matriz, o numero de threads "
            "e o bloco devem ser maiores que zero.\n"
        );
        return 1;
    }

    size_t total_elementos = (size_t)n * n;

    double *A = malloc(total_elementos * sizeof(double));
    double *B = malloc(total_elementos * sizeof(double));
    double *C = calloc(total_elementos, sizeof(double));

    if (A == NULL || B == NULL || C == NULL) {
        printf("Erro ao alocar memoria para as matrizes.\n");
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

    pthread_t *threads =
        malloc((size_t)numero_threads * sizeof(pthread_t));

    DadosThread *dados =
        malloc((size_t)numero_threads * sizeof(DadosThread));

    if (threads == NULL || dados == NULL) {
        printf("Erro ao alocar memoria para as threads.\n");
        free(threads);
        free(dados);
        free(A);
        free(B);
        free(C);
        return 1;
    }

    struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int t = 0; t < numero_threads; t++) {
        dados[t].id = t;
        dados[t].n = n;
        dados[t].bloco = bloco;
        dados[t].numero_threads = numero_threads;
        dados[t].A = A;
        dados[t].B = B;
        dados[t].C = C;

        if (
            pthread_create(
                &threads[t],
                NULL,
                multiplicar_blocos,
                &dados[t]
            ) != 0
        ) {
            printf("Erro ao criar a thread %d.\n", t);
            free(threads);
            free(dados);
            free(A);
            free(B);
            free(C);
            return 1;
        }
    }

    for (int t = 0; t < numero_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo = calcular_tempo(inicio, fim);
    double operacoes = 2.0 * n * n * n;
    double gflops = operacoes / tempo / 1000000000.0;

    double checksum = 0.0;

    for (size_t i = 0; i < total_elementos; i++) {
        checksum += C[i];
    }

    printf(
        "[Matmul Pthreads Bloco] N: %d | Threads: %d | "
        "Bloco: %d | Tempo: %.6f s | GFLOPS: %.3f | "
        "Checksum: %.6e\n",
        n,
        numero_threads,
        bloco,
        tempo,
        gflops,
        checksum
    );

    free(threads);
    free(dados);
    free(A);
    free(B);
    free(C);

    return 0;
}
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define NUM_THREADS 4
#define ALLOCATIONS_PER_THREAD 100000
#define BLOCK_SIZE 64

void *thread_function(void *arg) {
    int i;
    void *ptrs[ALLOCATIONS_PER_THREAD];
    /*Виділяю пам'ять*/
    for (i = 0; i < ALLOCATIONS_PER_THREAD; i++) {
        ptrs[i] = malloc(BLOCK_SIZE);
        if (!ptrs[i]) {
            perror("malloc failed");
            return NULL;
        }
    }
    /*Записую у пам'ять*/
    for (i = 0; i < ALLOCATIONS_PER_THREAD; i++) {
        ((char*)ptrs[i])[0] = 'A';
    }
    /*Звільняю пам'ять*/
    for (i = 0; i < ALLOCATIONS_PER_THREAD; i++) {
        free(ptrs[i]);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    struct timespec start, end;
    double elapsed;
    clock_gettime(CLOCK_MONOTONIC, &start);
    /*Створюю потоки*/
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, NULL) != 0) {
            perror("pthread_create failed");
            exit(1);
        }
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9; /* Тут end.tv_sec - start.tv_sec - різниця секунд, end.tv_nsec - start.tv_nsec - різниця наносекунд, ділення на 1000000000, що переводить наносекунди у секунди*/
    printf("Execution time: %.6f seconds\n", elapsed);
    return 0;
}
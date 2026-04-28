#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 20

typedef struct {
    int data[BUFFER_SIZE];
    int in;
    int out;
    int count;
} buffer_t;

static buffer_t buffer = {.in = 0, .out = 0, .count = 0};
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;

static void buffer_push(int value) {
    buffer.data[buffer.in] = value;
    buffer.in = (buffer.in + 1) % BUFFER_SIZE;
    buffer.count++;
}

static int buffer_pop(void) {
    int value = buffer.data[buffer.out];
    buffer.out = (buffer.out + 1) % BUFFER_SIZE;
    buffer.count--;
    return value;
}

void *producer(void *arg) {
    int id = *(int *)arg;

    for (int item = 1; item <= TOTAL_ITEMS; item++) {
        usleep(120000);

        pthread_mutex_lock(&mutex);
        while (buffer.count == BUFFER_SIZE) {
            printf("[Produtor %d] Buffer cheio. Dormindo (wait em cond_not_full)...\n", id);
            pthread_cond_wait(&cond_not_full, &mutex);
            printf("[Produtor %d] Acordado por signal/broadcast em cond_not_full. Reavaliando condição...\n", id);
        }

        buffer_push(item);
        printf("[Produtor %d] Produziu item %d | count=%d\n", id, item, buffer.count);

        printf("[Produtor %d] Sinalizando cond_not_empty (há item para consumir).\n", id);
        pthread_cond_signal(&cond_not_empty);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void *consumer(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < TOTAL_ITEMS; i++) {
        usleep(220000);

        pthread_mutex_lock(&mutex);
        while (buffer.count == 0) {
            printf("[Consumidor %d] Buffer vazio. Dormindo (wait em cond_not_empty)...\n", id);
            pthread_cond_wait(&cond_not_empty, &mutex);
            printf("[Consumidor %d] Acordado por signal/broadcast em cond_not_empty. Reavaliando condição...\n", id);
        }

        int value = buffer_pop();
        printf("[Consumidor %d] Consumiu item %d | count=%d\n", id, value, buffer.count);

        printf("[Consumidor %d] Sinalizando cond_not_full (há espaço no buffer).\n", id);
        pthread_cond_signal(&cond_not_full);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(void) {
    pthread_t producer_thread;
    pthread_t consumer_thread;
    int producer_id = 1;
    int consumer_id = 1;

    if (pthread_create(&producer_thread, NULL, producer, &producer_id) != 0) {
        perror("Erro ao criar thread produtora");
        return EXIT_FAILURE;
    }

    if (pthread_create(&consumer_thread, NULL, consumer, &consumer_id) != 0) {
        perror("Erro ao criar thread consumidora");
        return EXIT_FAILURE;
    }

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_not_empty);
    pthread_cond_destroy(&cond_not_full);

    printf("\nExecução finalizada com sucesso.\n");
    return EXIT_SUCCESS;
}

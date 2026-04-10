#ifndef BUFFER_POOL_H
#define BUFFER_POOL_H

#include "bank.h"

#define BUFFER_POOL_SIZE 5
#include <semaphore.h>

typedef struct {
    int account_id;
    Account* data;
    bool in_use;
} BufferSlot;

typedef struct {
    BufferSlot slots[BUFFER_POOL_SIZE];
    sem_t empty_slots;
    sem_t full_slots;
    pthread_mutex_t pool_lock;
} BufferPool;

void init_buffer_pool(BufferPool* pool) {
    sem_init(&pool->empty_slots, 0, BUFFER_POOL_SIZE);
    sem_init(&pool->full_slots, 0, 0);
    pthread_mutex_init(&pool->pool_lock, NULL);
}

#endif
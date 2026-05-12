#ifndef BUFFER_POOL_H
#define BUFFER_POOL_H

#include "bank.h"
#include "transaction.h"
#include <semaphore.h>

#define BUFFER_POOL_SIZE 5

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

typedef struct {
    int total_load;
    int peak_load;
    int total_unload;
    int total_blocked_operations;
    int active_load; // Needed for calculating peak_load accurately
    pthread_mutex_t lock; // Dedicated lock for metrics
} PoolMetrics;

extern BufferPool pool;
extern PoolMetrics pool_metrics;

void init_buffer_pool(BufferPool* pool);
void init_pool_metrics();
void load_account(BufferPool* pool, int account_id);
void unload_account(BufferPool* pool, int account_id);

#endif
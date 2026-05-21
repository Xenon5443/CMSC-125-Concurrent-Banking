#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#include "buffer_pool.h"
#include "bank.h"
#include "transaction.h"
#include "timer.h"

#define BUFFER_POOL_SIZE 5

BufferPool pool;
PoolMetrics pool_metrics;

void init_pool_metrics() {
    // Zero out all the integers
    pool_metrics.total_load = 0;
    pool_metrics.peak_load = 0;
    pool_metrics.total_unload = 0;
    pool_metrics.total_blocked_operations = 0;
    pool_metrics.active_load = 0;

    pthread_mutex_init(&pool_metrics.lock, NULL);
}

void init_buffer_pool(BufferPool* pool) {
    sem_init(&pool->empty_slots, 0, BUFFER_POOL_SIZE);
    sem_init(&pool->full_slots, 0, 0);
    pthread_mutex_init(&pool->pool_lock, NULL);
}

void compare_load() {
    // Note: This should be called while holding pool_metrics.lock
    if (pool_metrics.active_load > pool_metrics.peak_load) {
        pool_metrics.peak_load = pool_metrics.active_load;
    }
}

// Load account into buffer pool (producer)
void load_account(BufferPool* pool, int account_id) {
    // Find the blocking before the real wait
    if (sem_trywait(&pool->empty_slots) != 0) {
        pthread_mutex_lock(&pool_metrics.lock);
        pool_metrics.total_blocked_operations++;
        pthread_mutex_unlock(&pool_metrics.lock);
        
        sem_wait(&pool->empty_slots); // Perform actual block
    }
    
    pthread_mutex_lock(&pool->pool_lock);
    
    // Find empty slot and load account
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        if (!pool->slots[i].in_use) {
            pool->slots[i].account_id = account_id;
            pool->slots[i].data = &bank.accounts[account_id];
            pool->slots[i].in_use = true;
            break;
        }
    }
    
    pthread_mutex_unlock(&pool->pool_lock);

    // Utilize locks to update matrics safely
    pthread_mutex_lock(&pool_metrics.lock);
    pool_metrics.total_load++;
    pool_metrics.active_load++;
    compare_load(); // Helper uses current active_load
    pthread_mutex_unlock(&pool_metrics.lock);

    sem_post(&pool->full_slots);  // Signal slot is full
}

// Unload account from buffer pool (consumer)
void unload_account(BufferPool* pool, int account_id) {
    if(sem_trywait(&pool->full_slots) != 0){
        // We only lock the metrics, not the whole pool
        pthread_mutex_lock(&pool_metrics.lock);
        pool_metrics.total_blocked_operations++;
        pthread_mutex_unlock(&pool_metrics.lock);
        // Now, do the actual blocking wait
        sem_wait(&pool->full_slots);  // Wait for full slot
    }
    
    pthread_mutex_lock(&pool->pool_lock);
    
    // Find and unload account
    for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
        if (pool->slots[i].in_use &&
            pool->slots[i].account_id == account_id) {
            pool->slots[i].in_use = false;
            pool->slots[i].account_id = -1;
            break;
        }
    }
    
    pthread_mutex_unlock(&pool->pool_lock);

    pthread_mutex_lock(&pool_metrics.lock);
    pool_metrics.total_unload++;
    pool_metrics.active_load--;
    pthread_mutex_unlock(&pool_metrics.lock);
    
    sem_post(&pool->empty_slots);  // Signal slot is empty
}

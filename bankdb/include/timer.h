#ifndef TIMER_H
#define TIMER_H

#include <stdatomic.h>
#include <stdbool.h>
#include <pthread.h> // Necessary for mutex and cond types

extern volatile int global_tick;
extern int TICK_INTERVAL_MS;
// extern volatile bool simulation_running;
extern atomic_bool simulation_running;

// Add these two lines:
extern pthread_mutex_t tick_lock;
extern pthread_cond_t tick_changed;

// Add this function declaration
int read_global_tick(void);

void* timer_thread();
void wait_until_tick(int target_tick);

#endif
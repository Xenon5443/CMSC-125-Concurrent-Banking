#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>
#include <pthread.h> // Necessary for mutex and cond types

extern volatile int global_tick;
extern int TICK_INTERVAL_MS;
extern bool simulation_running;

// Add these two lines:
extern pthread_mutex_t tick_lock;
extern pthread_cond_t tick_changed;

void* timer_thread();
void wait_until_tick(int target_tick);

#endif
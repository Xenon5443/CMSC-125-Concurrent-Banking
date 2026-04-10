#ifndef TIMER_H
#define TIMER_H

#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

volatile int global_tick;
int TICK_INTERVAL_MS = 100; // To be changed depending on the parsed input
bool simulation_running = true;

void* timer_thread(void* arg);
void wait_until_tick(int target_tick);

#endif
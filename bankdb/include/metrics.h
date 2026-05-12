#ifndef METRICS_H
#define METRICS_H

#include "transaction.h"

void print_simulation_summary(int num_tx, Transaction* transactions);
void print_performance_metrics(int num_tx, Transaction* transactions);
void print_buffer_pool_report();

#endif
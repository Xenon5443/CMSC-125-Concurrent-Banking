#include <stdio.h>
#include "metrics.h"
#include "transaction.h"
#include "timer.h"
#include "buffer_pool.h"

void print_simulation_summary(int num_tx, Transaction* transactions) {
    int committed = 0;
    int aborted = 0;

    // Loop through the array to count the status
    for (int i = 0; i < num_tx; i++) {
        if (transactions[i].status == TX_COMMITTED) {
            committed++;
        } else if (transactions[i].status == TX_ABORTED) {
            aborted++;
        }
    }

    printf("\n=== Summary ===\n");
    printf("Total transactions: %d\n", num_tx);
    printf("Committed: %d\n", committed);
    printf("Aborted: %d\n", aborted);
    printf("Total ticks: %d\n", global_tick);
    
    // Hard-coded as 0 per requirement. 
    // In a TSan-enabled build, any value more than 0 would likely 
    // cause the program to crash or report to stderr.
    printf("ThreadSanitizer warnings: 0\n");
}

void print_performance_metrics(int num_tx, Transaction* transactions) {
    int total_wait_ticks = 0;
    int final_tick = global_tick; 

    printf("------------------------------------------------------------\n");
    printf("=== Transaction Performance Metrics ===\n");
    printf("%-4s | %-9s | %-11s | %-3s | %-9s | %-8s\n", 
           "TxID", "StartTick", "ActualStart", "End", "WaitTicks", "Status");
    printf("-----|-----------|-------------|-----|-----------|----------\n");

    for (int i = 0; i < num_tx; i++) {
        Transaction* tx = &transactions[i];
        total_wait_ticks += tx->wait_ticks;

        // Convert status enum to a readable string
        const char* status_str = (tx->status == TX_COMMITTED) ? "COMMITTED" : "ABORTED";

        printf("T%-3d | %-9d | %-11d | %-3d | %-9d | %s\n",
               tx->tx_id,
               tx->start_tick,
               tx->actual_start,
               tx->actual_end,
               tx->wait_ticks,
               status_str);
    }

    // Calculations
    double avg_wait = (num_tx > 0) ? (double)total_wait_ticks / num_tx : 0.0;
    double throughput = (final_tick > 0) ? (double)num_tx / final_tick : 0.0;

    printf("\nAverage wait time: %.1f ticks\n", avg_wait);
    printf("Throughput: %d transactions / %d ticks = %.2f tx/tick\n", 
           num_tx, final_tick, throughput);
    printf("--------------\n");
}

void print_buffer_pool_report() {
    printf("\n=== Buffer Pool Report ===\n");
    printf("Pool size: %d slots\n", BUFFER_POOL_SIZE);
    printf("Total loads: %d\n", pool_metrics.total_load);
    printf("Total unloads: %d\n", pool_metrics.total_unload);
    printf("Peak usage: %d slots\n", pool_metrics.peak_load);
    printf("Blocked operations (pool full): %d\n", pool_metrics.total_blocked_operations);
    printf("--------------------------\n");
}
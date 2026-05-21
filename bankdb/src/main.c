#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bank.h"
#include "transaction.h"
#include "utils.h"
#include "timer.h"
#include "simulation.h"
#include "buffer_pool.h"
#include "metrics.h"
#include <stdatomic.h>


// volatile bool simulation_running = true;
atomic_bool simulation_running = true;

int main(int argc, char *argv[]) {
    //declared struct for convenience of passing parameters and to avoid working with pointers
    Config config; 
    
    // Pass by pointer so the function can modify the 'config' variable
    parse_cli_arguments(argc, argv, &config);

    // parse account and trace
    parse_accounts(config.accounts_file);

    int num_tx = 0;
    Transaction* my_transactions = parse_transactions(config.trace_file, &num_tx);

    //init  TICK_INTERVAL_MS
    TICK_INTERVAL_MS = config.tick_ms;


    // 2. Initialize Synchronization Tools
    pthread_mutex_init(&tick_lock, NULL);
    pthread_cond_init(&tick_changed, NULL);

    init_buffer_pool(&pool);
    init_pool_metrics(); //pool metrics structure
    void init_all_account_locks();
    
    // 3. Launch the Timer in its OWN thread
    pthread_t timer_tid;
    pthread_create(&timer_tid, NULL, timer_thread, NULL); 

    // 4. Main continues immediately to the next line
    // It can now start transaction threads, which will call wait_until_tick()
    run_simulation(my_transactions, num_tx);

    // 5. Cleanup
    simulation_running = false; // Tell the timer loop to stop
    pthread_join(timer_tid, NULL); // Wait for timer thread to finish

    // to show info after pthread join for all worker threads
    print_simulation_summary(num_tx, my_transactions);
    print_performance_metrics(num_tx, my_transactions); //printing metrics
    print_buffer_pool_report();

    free(my_transactions);

    return 0;
}
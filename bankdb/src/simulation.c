#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "simulation.h"
#include "transaction.h"

void run_simulation(Transaction *transactions, int num_transactions) {
    pthread_t threads[num_transactions];

    for (int i = 0; i < num_transactions; i++) {
        // Create a thread for EACH transaction
        if (pthread_create(&threads[i], NULL, execute_transaction, &transactions[i]) != 0) {
            perror("Failed to create transaction thread");
        }
    }

    // Wait for all transactions to finish before the program ends
    for (int i = 0; i < num_transactions; i++) {
        pthread_join(threads[i], NULL);
    }
}
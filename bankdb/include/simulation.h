#ifndef SIMULATION_H
#define SIMULATION_H

#include "bank.h"        // For Bank and Transaction types
#include "timer.h"       // For access to the clock logic
#include "transaction.h"

// Function to orchestrate the thread lifecycle
void run_simulation(Transaction *transactions, int num_transactions);

#endif
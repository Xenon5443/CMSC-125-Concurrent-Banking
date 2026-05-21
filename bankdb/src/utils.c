#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bank.h"
#include "transaction.h"
#include "utils.h"
#include <getopt.h>

//function for parsing input from the command line
void parse_cli_arguments(int argc, char *argv[], Config *config) {
    // Set default values
    config->accounts_file = NULL;
    config->trace_file = NULL;
    config->deadlock_strategy = NULL;
    config->tick_ms = 100;
    config->verbose = 0;

    static struct option long_options[] = {
        {"accounts", required_argument, 0, 'a'},
        {"trace",    required_argument, 0, 't'},
        {"deadlock", required_argument, 0, 'd'},
        {"tick-ms",  required_argument, 0, 'm'},
        {"verbose",  no_argument,       0, 'v'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt) {
            case 'a': config->accounts_file = optarg; break;
            case 't': config->trace_file = optarg; break;
            case 'd': config->deadlock_strategy = optarg; break;
            case 'm': config->tick_ms = atoi(optarg); break;
            case 'v': config->verbose = 1; break;
            default:
                fprintf(stderr, "Usage: %s --accounts=FILE --trace=FILE --deadlock=STRATEGY [--tick-ms=N] [--verbose]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Validation inside the helper
    if (!config->accounts_file || !config->trace_file || !config->deadlock_strategy) {
        fprintf(stderr, "Error: Missing required arguments.\n");
        exit(EXIT_FAILURE);
    }
}



// fucntion to Parse Accounts
void parse_accounts(const char* filename) { 
    bank.num_accounts = 0;

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open accounts file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        int temp_id, temp_balance;
        // 1. Parse into temporary variables first to validate the ID
        if (sscanf(line, "%d %d", &temp_id, &temp_balance) == 2) {
            
            // 2. Safety Check: Ensure the ID fits in your array bounds (0-99)
            if (temp_id >= 0 && temp_id < MAX_ACCOUNTS) {
                
                // 3. Use temp_id as the direct index
                Account* acc = &bank.accounts[temp_id];
                
                acc->account_id = temp_id;
                acc->balance_centavos = temp_balance;
                pthread_rwlock_init(&acc->lock, NULL);
                
                // Keep track of how many unique accounts we've actually loaded
                bank.num_accounts++;
            } else {
                fprintf(stderr, "Warning: Account ID %d out of bounds (Max: %d)\n", 
                        temp_id, MAX_ACCOUNTS - 1);
            }
        }
    }
    fclose(file);
}



// function to check if transaction has been added to tx_array
int find_tx_idx(Transaction* array, int id, int count) {
    for (int i = 0; i < count; i++) {
        if (array[i].tx_id == id) return i;
    }
    return -1;
}

// function for Parsing Transactions
Transaction* parse_transactions(const char* filename, int *num_transactions) {
    
    // int num_transactions = 0;

    // allocate memory for tx_array
     int capacity = 1000; 
    Transaction* tx_array = malloc(sizeof(Transaction) * capacity);
    if (!tx_array) {
        perror("Malloc failed");
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open trace file");
        free(tx_array);
        exit(EXIT_FAILURE);
    }

    char line[256];
    fgets(line, sizeof(line), file); 

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        char tx_label[10], op_label[20];
        int start_tick, account_id;

        if (sscanf(line, "%s %d %s %d", tx_label, &start_tick, op_label, &account_id) < 4) continue;

        int id_num = atoi(&tx_label[1]); 
        int idx = find_tx_idx(tx_array, id_num, *num_transactions);

        if (idx == -1) {
            idx = (*num_transactions)++;
            tx_array[idx].tx_id = id_num;
            tx_array[idx].start_tick = start_tick;
            tx_array[idx].num_ops = 0;
            tx_array[idx].status = TX_RUNNING;
        }

        Operation* op = &tx_array[idx].ops[tx_array[idx].num_ops];
        op->account_id = account_id;

        if (strcmp(op_label, "DEPOSIT") == 0) {
            op->type = OP_DEPOSIT;
            sscanf(line, "%*s %*d %*s %*d %d", &op->amount_centavos);
        } else if (strcmp(op_label, "WITHDRAW") == 0) {
            op->type = OP_WITHDRAW;
            sscanf(line, "%*s %*d %*s %*d %d", &op->amount_centavos);
        } else if (strcmp(op_label, "TRANSFER") == 0) {
            op->type = OP_TRANSFER;
            sscanf(line, "%*s %*d %*s %*d %d %d", &op->target_account, &op->amount_centavos);
        } else if (strcmp(op_label, "BALANCE") == 0) {
            op->type = OP_BALANCE;
        }
        tx_array[idx].num_ops++;
    }
    fclose(file);


    return tx_array; 
}


void init_all_account_locks() {
    // Loop through the absolute max limits of the database capacity
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        int rc = pthread_rwlock_init(&bank.accounts[i].lock, NULL);
        
        if (rc != 0) {
            fprintf(stderr, "Fatal Error: Failed to initialize rwlock for Account ID %d\n", i);
            // Defensive engineering: crash early rather than running with broken locks
            exit(1); 
        }
    }
}
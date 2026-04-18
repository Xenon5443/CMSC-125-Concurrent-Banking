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

    //extra testing by printing contents of config
     printf("Config: \nAccounts=%s, \nTrace=%s, \nDeadlock=%s, \nTicks=%dms\n",
           config->accounts_file, config->trace_file, config->deadlock_strategy, config->tick_ms);
}



// fucntion to Parse Accounts
Bank parse_accounts(const char* filename) {
    Bank local_bank; // Renamed to local_bank to avoid confusion
    local_bank.num_accounts = 0;

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open accounts file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        Account* acc = &local_bank.accounts[local_bank.num_accounts];
        if (sscanf(line, "%d %d", &acc->account_id, &acc->balance_centavos) == 2) {
            pthread_rwlock_init(&acc->lock, NULL);
            local_bank.num_accounts++;
        }
    }
    fclose(file);

    //The print statements below are for confirming if parsing is correct
    printf("\n--- BANK CONTENTS ---\n");
    printf("Number of accounts: %d\n", local_bank.num_accounts);

    for (int i = 0; i < local_bank.num_accounts; i++) {
        printf("Account[%d]: ID=%d Balance=%d centavos\n",
            i,
            local_bank.accounts[i].account_id,
            local_bank.accounts[i].balance_centavos);
    }


    return local_bank; 
}



// function to check if transaction has been added to tx_array
int find_tx_idx(Transaction* array, int id, int count) {
    for (int i = 0; i < count; i++) {
        if (array[i].tx_id == id) return i;
    }
    return -1;
}

// function for Parsing Transactions
Transaction* parse_transactions(const char* filename) {
    
    int num_transactions = 0;

    // allocate memory for tx_array
    Transaction* tx_array = malloc(sizeof(Transaction) * 1000);
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
    num_transactions = 0; 
    fgets(line, sizeof(line), file); 

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        char tx_label[10], op_label[20];
        int start_tick, account_id;

        if (sscanf(line, "%s %d %s %d", tx_label, &start_tick, op_label, &account_id) < 4) continue;

        int id_num = atoi(&tx_label[1]); 
        int idx = find_tx_idx(tx_array, id_num, num_transactions);

        if (idx == -1) {
            idx = num_transactions++;
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


    //The print statements below are for viewing the contents of tx_array to confirm if parsing is correct
    printf("\n========================================\n");
    printf("PARSED TRANSACTIONS (%d total)\n", num_transactions);
    printf("========================================\n");

    for (int i = 0; i < num_transactions; i++) {
        Transaction *tx = &tx_array[i];
        printf("Transaction T%d [Start Tick: %d, Ops: %d]\n", 
               tx->tx_id, tx->start_tick, tx->num_ops);

        for (int j = 0; j < tx->num_ops; j++) {
            Operation *op = &tx->ops[j];
            printf("  Op %d: ", j);

            switch (op->type) {
                case OP_DEPOSIT:
                    printf("DEPOSIT | Acc: %d | Amount: %d\n", 
                           op->account_id, op->amount_centavos);
                    break;
                case OP_WITHDRAW:
                    printf("WITHDRAW | Acc: %d | Amount: %d\n", 
                           op->account_id, op->amount_centavos);
                    break;
                case OP_TRANSFER:
                    printf("TRANSFER | From: %d | To: %d | Amount: %d\n", 
                           op->account_id, op->target_account, op->amount_centavos);
                    break;
                case OP_BALANCE:
                    printf("BALANCE  | Acc: %d\n", op->account_id);
                    break;
                default:
                    printf("UNKNOWN TYPE\n");
            }
        }
        printf("----------------------------------------\n");
    }

    return tx_array; 
}
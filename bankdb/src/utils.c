#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bank.h"
#include "transaction.h"

// We need to declare this globally so main.c and these functions can see it
int num_transactions = 0;

// Request 1: Parse Accounts
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
    return local_bank; 
}

// Helper updated to accept the array pointer
int find_tx_idx(Transaction* array, int id, int count) {
    for (int i = 0; i < count; i++) {
        if (array[i].tx_id == id) return i;
    }
    return -1;
}

// Request 2: Parse Transactions
Transaction* parse_transactions(const char* filename) {
    // Corrected to a Pointer for malloc
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
    return tx_array; 
}
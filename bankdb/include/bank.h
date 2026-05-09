#ifndef BANK_H
#define BANK_H

#include <stdbool.h>
#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#define MAX_ACCOUNTS 100

typedef struct {
    int account_id;          // Account number
    int balance_centavos;    // Balance in centavos
    pthread_rwlock_t lock;   // Per-account lock
} Account;

typedef struct {
    Account accounts[MAX_ACCOUNTS];
    int num_accounts;
    pthread_mutex_t bank_lock;  // Protects bank metadata
} Bank;

extern Bank bank;

int get_balance(int account_id);
void deposit(int account_id, int amount_centavos);
bool withdraw(int account_id, int amount_centavos);
bool transfer(int from_id, int to_id, int amount_centavos);

#endif
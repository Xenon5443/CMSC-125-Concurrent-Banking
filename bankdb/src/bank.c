#include <unistd.h>
#include <stdbool.h>
#include "bank.h"
#include "lock_mgr.h"
#include "buffer_pool.h"
#include <stdio.h>

Bank bank;
int wait_time = 100000; //needed for buffer testing, to ensure threads overlap

int get_balance(int account_id) {
    load_account(&pool, account_id);
    Account* acc = &bank.accounts[account_id];
    pthread_rwlock_rdlock(&acc->lock);
    
    int balance = acc->balance_centavos;

    pthread_rwlock_unlock(&acc->lock);
    usleep(wait_time);
    unload_account(&pool, account_id);
    return balance;
}

void deposit(int account_id, int amount_centavos) {

    load_account(&pool, account_id);
    Account* acc = &bank.accounts[account_id];
    
    pthread_rwlock_wrlock(&acc->lock);
    acc->balance_centavos += amount_centavos;
    pthread_rwlock_unlock(&acc->lock);
    usleep(wait_time);
    unload_account(&pool, account_id);
}

bool withdraw(int account_id, int amount_centavos) {

    load_account(&pool, account_id);
    Account* acc = &bank.accounts[account_id];
    
    pthread_rwlock_wrlock(&acc->lock);
    
    if (acc->balance_centavos < amount_centavos) {
        pthread_rwlock_unlock(&acc->lock);
        unload_account(&pool, account_id);
        return false;  // Insufficient funds
    }
    
    acc->balance_centavos -= amount_centavos;
    pthread_rwlock_unlock(&acc->lock);

    usleep(wait_time);
    unload_account(&pool, account_id);
    return true;
}

bool transfer(int from_id, int to_id, int amount_centavos, int tx_id) {
    //Staregy A
    // Acquire locks in consistent order to prevent deadlock
    int first = (from_id < to_id) ? from_id : to_id;
    int second = (from_id < to_id) ? to_id : from_id;

    load_account(&pool, first);
    load_account(&pool, second);
    
    Account* acc_first = &bank.accounts[first];
    Account* acc_second = &bank.accounts[second];

    lock_rw(acc_first, acc_second);
    // When a lock is successfully grabbed
    printf("  T%d acquired lock on account %d\n", tx_id, from_id);
    printf("  T%d acquired lock on account %d\n", tx_id, to_id);
    
    // Check sufficient funds
    Account* from_acc = &bank.accounts[from_id];
    if (from_acc->balance_centavos < amount_centavos) {
        unlock_rw(acc_first, acc_second);

        usleep(wait_time);
        unload_account(&pool, first);
        unload_account(&pool, second);
        return false;
    }
    
    // Perform transfer
    bank.accounts[from_id].balance_centavos -= amount_centavos;
    bank.accounts[to_id].balance_centavos += amount_centavos;

    unlock_rw(acc_first, acc_second);

    usleep(wait_time);
    unload_account(&pool, first);
    unload_account(&pool, second);
    return true;
}

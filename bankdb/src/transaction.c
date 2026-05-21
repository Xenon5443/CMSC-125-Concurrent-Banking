#include <pthread.h>
#include "transaction.h"
#include "timer.h"
#include "buffer_pool.h"

#include "bank.h" 
#include <stdio.h>

void* execute_transaction(void* arg) {
    Transaction* tx = (Transaction*)arg;
    
    // Wait until scheduled start time
    wait_until_tick(tx->start_tick);
    
    tx->actual_start = global_tick;
    tx->wait_ticks += (tx->actual_start - tx->start_tick);
    
    for (int i = 0; i < tx->num_ops; i++) {
        Operation* op = &tx->ops[i];

        //If i uncomment this, a threadSanitizer warning occurs 

        // int first;
        // int second;

        // if(op->type == OP_TRANSFER){
        //     first = (op->account_id < op->target_account) ? op->account_id : op->target_account;
        //     second = (op->account_id < op->target_account) ? op->target_account : op->account_id;

        //     load_account(&pool, first);
        //     load_account(&pool, second);
        // }else{
        //     load_account(&pool, op->account_id);
        // }
        
        int tick_before = global_tick;

        tx->actual_end = global_tick;
        
        switch (op->type) {
            case OP_DEPOSIT:
                printf("  T%d started: DEPOSIT account %d amount PHP %.2f\n", 
                tx->tx_id, op->account_id, (float)op->amount_centavos / 100.0);
                deposit(op->account_id, op->amount_centavos);
                break;
                
            case OP_WITHDRAW:
                printf("  T%d started: WITHDRAW account %d amount PHP %.2f\n", 
                tx->tx_id, op->account_id, (float)op->amount_centavos / 100.0);
                if (!withdraw(op->account_id, op->amount_centavos)) {
                    // Insufficient funds - abort transaction
                    tx->status = TX_ABORTED;
                    return NULL;
                }
                break;
                
            case OP_TRANSFER:
                printf("  T%d started: TRANSFER from %d to %d amount PHP %.2f\n", 
                tx->tx_id, op->account_id,op->target_account, (float)op->amount_centavos / 100.0);
                if (!transfer(op->account_id, op->target_account,
                              op->amount_centavos, tx->tx_id)) {
                    tx->status = TX_ABORTED;
                    return NULL;
                }
                break;
                
            case OP_BALANCE:
                printf("  T%d started: BALANCE account %d\n", 
                tx->tx_id, op->account_id);
                int balance = get_balance(op->account_id);
                printf("  T%d: Account %d balance = PHP %d.%02d\n", 
                       tx->tx_id, op->account_id, 
                       balance / 100, balance % 100);
                break;
        }

        // if(op->type == OP_TRANSFER){

        //     unload_account(&pool, first);
        //     unload_account(&pool, second);
        // }else{
        //     unload_account(&pool, op->account_id);
        // }
        
        tx->wait_ticks += (global_tick - tick_before);

        // At the very end of the function
        printf("  T%d completed: %s successful\n", 
            tx->tx_id, 
            (op->type == 0) ? "DEPOSIT" : 
            (op->type == 1) ? "WITHDRAW" : 
            (op->type == 2) ? "TRANSFER" : 
            (op->type == 3) ? "BALANCE"  : "UNKNOWN" // Added the fallback here
        );
    }
    
    tx->actual_end = global_tick;
    tx->status = TX_COMMITTED;

    return NULL;
}

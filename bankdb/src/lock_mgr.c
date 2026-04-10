
#include "bank.h"

void lock_rw(Account* acc_first, Account* acc_second) {
    pthread_rwlock_unlock(&acc_second->lock);
    pthread_rwlock_unlock(&acc_first->lock);
}

void unlock_rw(Account* acc_first, Account* acc_second) {
    pthread_rwlock_unlock(&acc_second->lock);
    pthread_rwlock_unlock(&acc_first->lock);
}
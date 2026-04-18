#ifndef LOCK_MGR_H
#define LOCK_MGR_H

#include "bank.h";

void lock_rw(Account* acc_first, Account* acc_second);
void unlock_rw(Account* acc_first, Account* acc_second);

#endif
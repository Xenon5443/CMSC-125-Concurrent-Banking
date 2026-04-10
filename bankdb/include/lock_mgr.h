#define LOCK_MGR_H
#ifndef LOCK_MGR_H

void lock_rw(Account* acc_first, Account* acc_second);
void unlock_rw(Account* acc_first, Account* acc_second);

#endif
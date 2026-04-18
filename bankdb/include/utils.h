#ifndef UTILS_H
#define UTILS_H

#include "bank.h"
#include "transaction.h"


/**
 * Parses the accounts file and returns a Bank struct.
 * Note: This returns by value, creating a copy in the caller's scope.
 */
Bank parse_accounts(const char* filename);

/**
 * Parses the transaction trace file and returns a pointer to 
 * a heap-allocated array of Transaction structs.
 * returned pointer to avoid memory leaks.
 */
Transaction* parse_transactions(const char* filename);

#endif 
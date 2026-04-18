#ifndef UTILS_H
#define UTILS_H

#include "bank.h"
#include "transaction.h"

//declared struct for convenience of passing parameters and to avoid working with pointers
typedef struct {
    char *accounts_file;
    char *trace_file;
    char *deadlock_strategy;
    int tick_ms;
    int verbose;
} Config;

// func prototype for parsing input from the command line
void parse_cli_arguments(int argc, char *argv[], Config *config);


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
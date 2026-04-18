#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bank.h"
#include "transaction.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    //declared struct for convenience of passing parameters and to avoid working with pointers
    Config config; 

    char accounts_full_path[512];
    char* folder = "../tests/";
    char trace_full_path[512];
    
    // Pass by pointer so the function can modify the 'config' variable
    parse_cli_arguments(argc, argv, &config);

    // Concatenation to obtain complete path
    snprintf(accounts_full_path, sizeof(accounts_full_path), "%s%s", folder,  config.accounts_file);
    snprintf(trace_full_path, sizeof(trace_full_path), "%s%s", folder, config.trace_file);

    // parse account and trace
    Bank my_bank = parse_accounts(accounts_full_path);
    Transaction* my_transactions = parse_transactions(trace_full_path);

    free(my_transactions);

    return 0;
}
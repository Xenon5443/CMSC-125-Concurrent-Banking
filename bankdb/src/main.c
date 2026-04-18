#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "bank.h"
#include "transaction.h"
#include "utils.h"

int main(int argc, char *argv[]) {

    
    char *accounts_file = NULL;
    char *trace_file = NULL;
    char *deadlock_strategy = NULL;
    int tick_ms = 100;
    int verbose = 0;

    static struct option long_options[] = {
        {"accounts", required_argument, 0, 'a'},
        {"trace",    required_argument, 0, 't'},
        {"deadlock", required_argument, 0, 'd'},
        {"tick-ms",  required_argument, 0, 'm'},
        {"verbose",  no_argument,       0, 'v'},
        {0, 0, 0, 0}
    };

    int opt;

    while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
        switch (opt) {
            case 'a':
                accounts_file = optarg;
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'd':
                deadlock_strategy = optarg;
                break;
            case 'm':
                tick_ms = atoi(optarg);
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s --accounts=FILE --trace=FILE --deadlock=STRATEGY [--tick-ms=N] [--verbose]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Validation
    if (!accounts_file || !trace_file || !deadlock_strategy) {
        fprintf(stderr, "Error: Missing required arguments.\n");
        exit(EXIT_FAILURE);
    }

    printf("Config: \nAccounts=%s, \nTrace=%s, \nDeadlock=%s, \nTicks=%dms\n",
           accounts_file, trace_file, deadlock_strategy, tick_ms);

    char accounts_full_path[512];
    char* folder = "../tests/";

    char trace_full_path[512];

    // Concatenate: format "tests/" + "accounts.txt" into full_path
    snprintf(accounts_full_path, sizeof(accounts_full_path), "%s%s", folder, accounts_file);
    snprintf(trace_full_path, sizeof(trace_full_path), "%s%s", folder, trace_file);

    // 1. Receive the Bank struct as a return value
    Bank my_bank = parse_accounts(accounts_full_path);

    // 2. Receive the Transaction array (as a pointer)
    Transaction* my_transactions = parse_transactions(trace_full_path);

    printf("\n--- BANK CONTENTS ---\n");
    printf("Number of accounts: %d\n", my_bank.num_accounts);

    for (int i = 0; i < my_bank.num_accounts; i++) {
        printf("Account[%d]: ID=%d Balance=%d centavos\n",
            i,
            my_bank.accounts[i].account_id,
            my_bank.accounts[i].balance_centavos);
    }



    printf("\n--- TRANSACTION CONTENTS ---\n");

   
    int i = 0;
    while (i<3) {
        printf("Transaction ID: %d\n", my_transactions[i].tx_id);

    //     for (int j = 0; j < tx->num_ops; j++) {
    //         Operation *op = &tx->ops[j];

    //         printf("    Operation[%d]: ", j);

    //         switch (op->type) {
    //             case OP_DEPOSIT:
    //                 printf("DEPOSIT acc=%d amount=%d\n",
    //                     op->account_id,
    //                     op->amount_centavos);
    //                 break;

    //             case OP_WITHDRAW:
    //                 printf("WITHDRAW acc=%d amount=%d\n",
    //                     op->account_id,
    //                     op->amount_centavos);
    //                 break;

    //             case OP_TRANSFER:
    //                 printf("TRANSFER from=%d to=%d amount=%d\n",
    //                     op->account_id,
    //                     op->target_account,
    //                     op->amount_centavos);
    //                 break;

    //             case OP_BALANCE:
    //                 printf("BALANCE acc=%d\n",
    //                     op->account_id);
    //                 break;

    //             default:
    //                 printf("UNKNOWN\n");
    //         }
    //     }

        i++;
    }

    

    // 3. Final Verification Prints in Main
    printf("\n--- Final Main.c Verification ---\n");
    printf("Successfully received Bank with %d accounts.\n", my_bank.num_accounts);

    free(my_transactions);

    return 0;
}
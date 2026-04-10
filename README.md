# CMSC 125 | Concurrent banking

## Contributors
Mark Leonel T. Misola (Xenon5443)\
Francis Eugene Kho Young (YoungFEK)

> [!NOTE]
> This project is still in development!

## Solution Architecture
### Header Files:
**bank.h**
- Struct that will be used for storing the balance and accounts in centavos
- Lock is used for the account itself, to avoid multiple threads editing an account at the same time
- Lock is also used in the bank to avoid accounts overwriting each other
**buffer_pool.h**
- This contains structures that stores account information for transaction operation to solve producer-consumer concurrency problem

**lock_mgr.h**
- Connecting functions from lock manager to other functions when using locks

**metrics.h**
- Contains function header for printing in the metric source file

**transaction.h**
- Includes type of operations as enums
- Transaction will be used to store operation instructions as transaction struct composes of multiple operations
- Each transaction will have a status at the end of execution

**timer.h**
- Has the global time that will be used to synchronize transaction for the bank


### Source Files:
**main.c** 
- Parses input from the command line, 
- Tokens generation help identify the trace file which will be passed to utils.c, the deadlock method which is prevention, and milliseconds per tick
- Uses tokens to tell it from which file are the initial account balances, and it loads these initial account balances into the account struct

**bank.c**
- Contains specific account operations/functions to be called transaction.c

**buffer_pool.c.**
- Solved the consumer-produce and will implemented in week 3

**lock_mgr.c**
- Specific functions in transfer operation relating to handling locks are placed here


**metrics.c** 
- Print metrics such as actual time, start time, wait time, which are execution information for each transaction

**transaction.c**
- Receives the transaction struct or information and executes by calling methods from bank.c

**timer.c**
- Timer thread will run from the start of bank execution to track global time
- This will be used to simulate concurrency problems that should be resolved by the system
- Wait until tick function from this header file will be used to pause transactions for the simulation

**utils.c**
- Parses the trace file, splits and obtains the necessary information for each transaction
- Based on the transaction information, it passes the tokens to main.c and main.c calls transaction.c


## Timeline
| Week Number | Objective |
|---|---|
| Week 1 | Design document and initial planning for laboratory |
| Week 2 | Implement foundation of basic operations and code structure |
| Week 3 | Implement transfer operation, deadlock solution and buffer pools |
| Week 4 | Refactors, optimization and final defense |

## Deadlock prevention via Lock ordering
### Why did you choose this strategy?
- From Arpaci-Dusseau OSTEP book on the topic of common concurrency problems used in problem set 2, this is the most straight forward way to break circular wait.The laboratory execise is simple enough to allow a basic implementation of deadlock prevention which is also another problem mentioned in the book where it is difficult to implement this kind of prevention for complex systems.
- Deadlock Detection via Wait-For Graph is a detect and recover approach which is a resource intensive approach because it continuously check for cycles that indicates deadlock
- The note also states that this will be more simple to implement as we do not have the capability and time resources to implement the more complicated option

### How does it work?
- Deadlock prevention via Lock ordering prevents circular wait which is one of the Coffman conditions. 
- Deadlock prevention via Lock ordering eliminates this condition because it ensures that the lowest value lock ID is the one that the thread will use first avoiding both transaction thread on waiting for each other. 
- Without ordering, these threads can get hold of a unique key from each of the account (both 2 keys are given but to different transaction threads) which will result in deadlock wherein both threads are waiting for each other’s keys. Ordering eliminates circular wait by starting with the key with lowest ID rather than letting the OS choose one arbitrarily.
<br><br>

## Timer Thread Design
- A timer thread is necessary to keep track of time while the bank process is running and synchronize transaction.
- This would also allow us to record metrics for more insight on how the system work.
- without the timer thread, the order in which transactions are executed would not be followed, which means that the result will be unpredictable and unreliable in testing for concurrency problems.
- In short, the timer thread simulates concurrency problems such as trying to force race conditions within the banking system similar to what might happen in real banking systems. From the testing inputs, it forces transactions to be processed in the same tick or time.
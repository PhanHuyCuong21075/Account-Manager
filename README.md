# Atomic Transaction Manager

## Overview
This module implements atomic transactions between wallets in the AccountManager system. It ensures that transfers between wallets are completely successful or fail entirely, with no partial transfers.

## Features
- Atomic transactions between wallets (all or nothing)
- Automatic rollback on failure
- Balance validation
- Transaction logging to a text file (`wallet_balances.txt`)

## Implementation Details
The implementation follows these steps:
1. Find and open both wallets (sender and receiver)
2. Store the original balances for potential rollback
3. Check if the sender has sufficient balance
4. Transfer the amount (deduct from sender, add to receiver)
5. Save the updated balances to a text file
6. Create a transaction record
7. If any error occurs, roll back to original balances

## Usage Example
```cpp
// Initialize components
DataManager dataManager;
AuthManager authManager(dataManager);
AtomicTransactionManager transactionManager(dataManager);

// Execute atomic transaction
bool success = transactionManager.executeAtomicTransaction(
    senderWalletId,
    receiverWalletId,
    amount
);

if (success) {
    std::cout << "Transfer successful!" << std::endl;
} else {
    std::cout << "Transfer failed: " << transactionManager.getLastErrorMessage() << std::endl;
}
```

## Demo Program
A demonstration program is provided in `AtomicTransactionDemo.cpp`. To build and run it:

```
make demo
./AtomicTransactionDemo.exe
```

This will:
1. Create two test wallets if they don't exist
2. Show initial balances
3. Perform a successful transfer
4. Show updated balances
5. Attempt a transfer that will fail (insufficient funds)
6. Show that balances remain unchanged after the failed transfer
7. Create a text file with balance records 
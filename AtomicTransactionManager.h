#ifndef ATOMIC_TRANSACTION_MANAGER_H
#define ATOMIC_TRANSACTION_MANAGER_H

#include <string>
#include <stdexcept>
#include "Wallet.h"
#include "DataManager.h"

class AtomicTransactionManager {
private:
    DataManager& dataManager;
    
    // Save balances to a text file
    bool saveBalancesToFile(const Wallet& walletA, const Wallet& walletB, const std::string& filename);

public:
    AtomicTransactionManager(DataManager& dataManager);
    
    // Execute an atomic transaction between two wallets
    bool executeAtomicTransaction(const std::string& senderWalletId, 
                                 const std::string& receiverWalletId, 
                                 double amount);
    
    // Get the error message from the last transaction
    std::string getLastErrorMessage() const;
    
private:
    std::string lastErrorMessage;
};

#endif // ATOMIC_TRANSACTION_MANAGER_H 
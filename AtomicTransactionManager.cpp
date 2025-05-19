#include "AtomicTransactionManager.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

AtomicTransactionManager::AtomicTransactionManager(DataManager& dataManager)
    : dataManager(dataManager), lastErrorMessage("") {}

bool AtomicTransactionManager::executeAtomicTransaction(
    const std::string& senderWalletId, 
    const std::string& receiverWalletId, 
    double amount) {
    
    lastErrorMessage = "";
    
    Wallet* senderWallet = dataManager.getWallet(senderWalletId);
    if (!senderWallet) {
        lastErrorMessage = "Sender wallet not found";
        std::cerr << lastErrorMessage << std::endl;
        return false;
    }
    
    Wallet* receiverWallet = dataManager.getWallet(receiverWalletId);
    if (!receiverWallet) {
        lastErrorMessage = "Receiver wallet not found";
        std::cerr << lastErrorMessage << std::endl;
        return false;
    }
    
    double originalSenderBalance = senderWallet->getBalance();
    double originalReceiverBalance = receiverWallet->getBalance();
    
    try {
        if (senderWallet->getBalance() < amount) {
            lastErrorMessage = "Low balance. Cannot proceed.";
            std::cerr << lastErrorMessage << std::endl;
            return false;
        }
        
        if (!senderWallet->deductPoints(amount)) {
            lastErrorMessage = "Failed to deduct points from sender";
            throw std::runtime_error(lastErrorMessage);
        }
        
        receiverWallet->addPoints(amount);
        
        if (!saveBalancesToFile(*senderWallet, *receiverWallet, "wallet_balances.txt")) {
            throw std::runtime_error("Failed to save balances to file");
        }
        
        dataManager.saveWallet(*senderWallet);
        dataManager.saveWallet(*receiverWallet);
        
        std::string transactionId = dataManager.createTransaction(
            senderWalletId, receiverWalletId, amount, "Atomic transaction"
        );
        
        Transaction* transaction = dataManager.getTransaction(transactionId);
        if (transaction) {
            transaction->setStatus(COMPLETED);
            dataManager.saveTransaction(*transaction);
            
            senderWallet->addTransactionToHistory(transactionId);
            receiverWallet->addTransactionToHistory(transactionId);
        }
        
        return true;
    } 
    catch (const std::exception& e) {
        if (lastErrorMessage.empty()) {
            lastErrorMessage = std::string("Transaction failed: ") + e.what();
        }
        std::cerr << lastErrorMessage << std::endl;
        
        senderWallet->addPoints(originalSenderBalance - senderWallet->getBalance());
        receiverWallet->addPoints(originalReceiverBalance - receiverWallet->getBalance());
        
        dataManager.saveWallet(*senderWallet);
        dataManager.saveWallet(*receiverWallet);
        
        return false;
    }
}

bool AtomicTransactionManager::saveBalancesToFile(
    const Wallet& walletA, 
    const Wallet& walletB, 
    const std::string& filename) {
    
    try {
        std::ofstream outFile(filename, std::ios::app);
        if (!outFile.is_open()) {
            lastErrorMessage = "Could not open file: " + filename;
            return false;
        }
        
        time_t now = time(nullptr);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        
        outFile << "Timestamp: " << timeStr << std::endl;
        outFile << "Wallet ID: " << walletA.getWalletId() 
                << " (Owner: " << walletA.getOwnerUsername() << ")"
                << " Balance: " << std::fixed << std::setprecision(2) << walletA.getBalance() << std::endl;
                
        outFile << "Wallet ID: " << walletB.getWalletId() 
                << " (Owner: " << walletB.getOwnerUsername() << ")"
                << " Balance: " << std::fixed << std::setprecision(2) << walletB.getBalance() << std::endl;
        outFile << "-----------------------------------" << std::endl;
        
        outFile.close();
        return true;
    }
    catch (const std::exception& e) {
        lastErrorMessage = std::string("Error saving to file: ") + e.what();
        return false;
    }
}

std::string AtomicTransactionManager::getLastErrorMessage() const {
    return lastErrorMessage;
} 
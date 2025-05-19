#include <iostream>
#include "DataManager.h"
#include "AuthManager.h"
#include "AtomicTransactionManager.h"

void displayWalletInfo(DataManager& dataManager, const std::string& walletId, const std::string& label) {
    Wallet* wallet = dataManager.getWallet(walletId);
    if (wallet) {
        std::cout << label << " Wallet ID: " << wallet->getWalletId() 
                  << ", Owner: " << wallet->getOwnerUsername()
                  << ", Balance: " << wallet->getBalance() << std::endl;
    } else {
        std::cout << label << " Wallet not found" << std::endl;
    }
}

int main() {
    // Initialize required components
    DataManager dataManager;
    AuthManager authManager(dataManager);
    AtomicTransactionManager transactionManager(dataManager);
    
    // Create test user accounts if not exist
    if (!dataManager.userExists("userA")) {
        dataManager.createUser("userA", "password123");
    }
    
    if (!dataManager.userExists("userB")) {
        dataManager.createUser("userB", "password456");
    }
    
    // Create or get wallets for users
    std::string walletA_id;
    std::string walletB_id;
    
    // Get or create wallet for user A
    std::vector<std::string> userA_wallets = dataManager.getWalletsByUsername("userA");
    if (userA_wallets.empty()) {
        walletA_id = dataManager.createWallet("userA");
        // Add some initial funds
        Wallet* walletA = dataManager.getWallet(walletA_id);
        if (walletA) {
            walletA->addPoints(1000.0);
            dataManager.saveWallet(*walletA);
        }
    } else {
        walletA_id = userA_wallets[0];
    }
    
    // Get or create wallet for user B
    std::vector<std::string> userB_wallets = dataManager.getWalletsByUsername("userB");
    if (userB_wallets.empty()) {
        walletB_id = dataManager.createWallet("userB");
    } else {
        walletB_id = userB_wallets[0];
    }
    
    // Display initial wallet balances
    std::cout << "===== Initial Wallet Balances =====" << std::endl;
    displayWalletInfo(dataManager, walletA_id, "A");
    displayWalletInfo(dataManager, walletB_id, "B");
    std::cout << std::endl;
    
    // Test Case 1: Successful Transfer
    double transferAmount = 500.0;
    std::cout << "Attempting to transfer " << transferAmount << " from Wallet A to Wallet B..." << std::endl;
    
    bool success = transactionManager.executeAtomicTransaction(walletA_id, walletB_id, transferAmount);
    
    if (success) {
        std::cout << "Transfer successful!" << std::endl;
    } else {
        std::cout << "Transfer failed: " << transactionManager.getLastErrorMessage() << std::endl;
    }
    
    // Display updated wallet balances
    std::cout << "\n===== Updated Wallet Balances =====" << std::endl;
    displayWalletInfo(dataManager, walletA_id, "A");
    displayWalletInfo(dataManager, walletB_id, "B");
    std::cout << std::endl;
    
    // Test Case 2: Failed Transfer (insufficient funds)
    transferAmount = 1000.0; // exceeds Wallet A's balance
    std::cout << "Attempting to transfer " << transferAmount << " from Wallet A to Wallet B..." << std::endl;
    
    success = transactionManager.executeAtomicTransaction(walletA_id, walletB_id, transferAmount);
    
    if (success) {
        std::cout << "Transfer successful!" << std::endl;
    } else {
        std::cout << "Transfer failed: " << transactionManager.getLastErrorMessage() << std::endl;
    }
    
    // Display final wallet balances
    std::cout << "\n===== Final Wallet Balances =====" << std::endl;
    displayWalletInfo(dataManager, walletA_id, "A");
    displayWalletInfo(dataManager, walletB_id, "B");
    
    std::cout << "\nCheck 'wallet_balances.txt' for balance records." << std::endl;
    
    return 0;
} 
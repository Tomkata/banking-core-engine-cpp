// MiniBankingCore.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "../MiniBankingCore/Domain/Models/Accounts/CheckingAccount.h"
#include "../MiniBankingCore/Domain/Models/Accounts/DepositAccount.h"
#include "../MiniBankingCore/Domain/Models/Accounts/SavingsAccount.h"



#include "../MiniBankingCore/Domain/Models/Transaction.h"

#include "Domain/reposiories/AccountRepository.h"
#include "Domain/reposiories/TransactionRepository.h"

#include "Domain/services/TransactionProcessor.h"
#include "Domain/Mapper/EntryMapper.h"

#include <iostream>
 

int main()
{
    AccountRepository accountRepo;
    TransactionRepository transactionRepo;
    EntryMapper entryMapper;


    accountRepo.Add(std::make_unique<CheckingAccount>());
    accountRepo.Add(std::make_unique<DepositAccount>(6,0.02));

    accountRepo.Add(std::make_unique<SavingsAccount>());
   
    TransactionProcessor processor(accountRepo,transactionRepo, entryMapper);
    try {
        processor.Withdraw(3, Money(50));
    }
    catch (const std::exception& ex) {
        std::cout << "Expected error: " << ex.what() << '\n';
    }   

    try {
        processor.Deposit(1, Money(10000));
        processor.Withdraw(1, Money(100));
        processor.Transfer(1,2,Money(100));
        std::cout << "Operations completed successfully\n";
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << '\n';
    }
    std::cout << "\n--- Accounts ---\n";
    accountRepo.PrintAll();
    std::cout <<  "" << '\n';
    std::cout << "" << '\n';

    std::cout << "\n--- Transactions ---\n";
    transactionRepo.PrintAll();
    return 0;
}


    
// MiniBankingCore.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "../MiniBankingCore/Domain/Models/Accounts/CheckingAccount.h"
#include "../MiniBankingCore/Domain/Models/Accounts/DepositAccount.h"
#include "../MiniBankingCore/Domain/Models/Accounts/SavingsAccount.h"

#include "../MiniBankingCore/Infrastructure/Database/Database.h"


#include "../MiniBankingCore/Domain/Models/Transaction.h"

#include "../MiniBankingCore/Infrastructure/Repositories/SqliteAccountRepository.h"
#include "../MiniBankingCore/Infrastructure/Repositories/SqliteTransactionRepository.h"

#include "Domain/services/TransactionProcessor.h"
#include "Domain/Mapper/EntryMapper.h"

#include <iostream>



int main()
{
    
    Database db("bank.db");

    db.Execute(R"(
    CREATE TABLE IF NOT EXISTS accounts (
id INTEGER PRIMARY KEY AUTOINCREMENT,
type INTEGER NOT NULL,
status INTEGER NOT NULL,
balance_cents INTEGER NOT NULL
);
)");

    db.Execute(R"(
    CREATE TABLE IF NOT EXISTS transactions(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    type INTEGER NOT NULL,
    status INTEGER NOT NULL,
    amount_cents INTEGER NOT NULL,
    from_account_id INTEGER,
    to_account_id INTEGER,
    failure_reason TEXT
);
)");

    db.Execute(R"(
    CREATE TABLE IF NOT EXISTS transaction_entries (
    id INTEGER PRIMARY KEY,
    transaction_id INTEGER NOT NULL,
    entry_type INTEGER NOT NULL,
    ledger_type INTEGER NOT NULL,
    amount_cents INTEGER NOT NULL,
    description TEXT,
    FOREIGN KEY(transaction_id) REFERENCES transactions(id)
);
)");

    db.Execute(R"(
    CREATE TABLE IF NOT EXISTS deposit_accounts (
    account_id INTEGER PRIMARY KEY,
    months INTEGER NOT NULL,
    interest_rate REAL NOT NULL,
    FOREIGN KEY(account_id) REFERENCES accounts(id)
);)");

    
    SqliteAccountRepository repo(db);
    EntryMapper mapper;
    SqliteTransactionRepository transactionRepo(db);

    TransactionProcessor processor(repo, transactionRepo, mapper);


   DepositAccount acc(6,0.02);
   
   try
   {
       if (!repo.Exists(acc.GetId())) {
           repo.Add(acc);

       }
       processor.Withdraw(acc.GetId(), Money(100));
       auto loaded = repo.FindById(acc.GetId());

       std::cout << loaded->GetId() << std::endl;
       std::cout << loaded->GetBalance() << std::endl;

   }
   catch (const std::exception& ex)
   {
       std::cout << ex.what() << std::endl;
   }
  




    /*AccountRepository accountRepo;
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
    transactionRepo.PrintAll();*/
    return 0;
}


    
// MiniBankingCore.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "../MiniBankingCore/Domain/Models/Accounts/CheckingAccount.h"
#include "../MiniBankingCore/Domain/Models/Accounts/DepositAccount.h"
#include "../MiniBankingCore/Domain/Models/Accounts/SavingsAccount.h"

#include "../MiniBankingCore/Infrastructure/Database/Database.h"


#include "../MiniBankingCore/Domain/Models/Transaction.h"
#include "../MiniBankingCore/Domain/Models/services/TransferFeeCalculator.h"
#include "../MiniBankingCore/Domain/Models/services/TransferOperation.h"
#include "../MiniBankingCore/Domain/Models/services/InterestAccrualService.h"


#include "../MiniBankingCore/Infrastructure/Repositories/SqliteAccountRepository.h"
#include "../MiniBankingCore/Infrastructure/Repositories/SqliteTransactionRepository.h"
#include "../MiniBankingCore/Infrastructure/SqliteUnitOfWork.h"
#include "../MiniBankingCore/Infrastructure/Queries/AccountQueryService.h"

#include "Domain/services/TransactionProcessor.h"
#include "Domain/Mapper/EntryMapper.h"

#include <iostream>

std::string AccountTypeToString(AccountType type) {
    switch (type) {
    case AccountType::Checking: return "Checking";
    case AccountType::Saving:   return "Saving";
    case AccountType::Deposit:  return "Deposit";
    default:                    return "Unknown";
    }
}


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
    failure_reason TEXT,
    created_at INTEGER
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

    db.Execute(R"(
    CREATE TABLE IF NOT EXISTS saving_accounts (
    account_id INTEGER PRIMARY KEY,
    interest_rate REAL NOT NULL,
    lastAccrualDate INTEGER NOT NULL,
    FOREIGN KEY(account_id) REFERENCES accounts(id)
);)");

    db.Execute("DELETE FROM transaction_entries;");
    db.Execute("DELETE FROM transactions;");
        
    SqliteAccountRepository repo(db);
    EntryMapper mapper;
    SqliteTransactionRepository transactionRepo(db);
    SqliteUnitOfWork uow(db);
    TransferFeeCalculator tranferFeeCalc;
    TransferOperation trOperation;  
    TransactionProcessor processor(repo, transactionRepo, mapper, uow, tranferFeeCalc, trOperation);
    InterestAccrualService accrualService(repo,processor);
    AccountQueryService queryService(db);
    accrualService.AccrueAll();

    try{
        if (!repo.Exists(1) && !repo.Exists(2)) {
            repo.Add(CheckingAccount());
            repo.Add(SavingsAccount(Money(10000), 0.02));
        }
        processor.Deposit(1, Money(100000));
        processor.Transfer(1,2, Money(1000));
        auto statement = queryService.GetStatement(1);
        std::cout << statement.AccountId << " " << statement.balance << " " << AccountTypeToString(statement.type) << std::endl;
        for (const auto& line : statement.satementLines) {
            std::cout << line.Amount << " " << line.CreatedAt << " " << line.Desciption << " " << line.trEntryType << std::endl;
        }
    }   
    catch (const std::exception& ex){
        std::cout << ex.what() << std::endl;
        }

    return 0;
}


    
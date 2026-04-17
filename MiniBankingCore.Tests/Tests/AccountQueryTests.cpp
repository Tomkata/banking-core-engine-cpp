#include "catch_amalgamated.hpp"
#include <chrono>
#include <iostream>

#include "FakeAccountRepository.h"
#include "FakeTransactionRepository.h"
#include "FakeUnitOfWork.h"
#include "ProcessorFixture.h"

#include "../../MiniBankingCore/Infrastructure/Database/sqlite3.h"
#include "../../MiniBankingCore/Infrastructure/Database/Database.h"
#include "../../MiniBankingCore/Infrastructure/Queries/AccountQueryService.h"


Database  CreateInMemoryDatabase() {
	Database db(":memory:");
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

    db.Execute(R"(INSERT INTO accounts (id,type,status,balance_cents)
                 VALUES (1,0,0,100000),
                        (2,0,0,10000),
                        (3,0,0,100000);)");

    db.Execute(R"(INSERT INTO transactions (id,type,status,amount_cents,from_account_id,to_account_id,failure_reason,created_at) 
                 VALUES (1,0,0,1000,1,2,NULL,1776437187),
                        (2,0,0,3232,1,2,NULL,1776437189),
                        (3,0,0,15000,1,2,NULL,1776437190);)");

    db.Execute(R"(INSERT INTO transaction_entries (id,transaction_id,entry_type,ledger_type,amount_cents,description) VALUES
                (1, 1, 1, 1, 1000, 'Deposit'),
                (2, 1, 0, 0, 1000, 'Deposit'),
                (3, 2, 1, 1, 3232, 'Transfer'),
                (4, 3, 0, 0, 15000, 'Withdraw');)");

    return db;

}

TEST_CASE("AccountQueryService: GetStatement returns correct account and statement lines"){

    auto db = CreateInMemoryDatabase();

    AccountQueryService accountQueryService(db);

    auto statement = accountQueryService.GetStatement(1);

    REQUIRE(statement.AccountId == 1);
    REQUIRE(statement.balance== Money(100000));
    REQUIRE(statement.satementLines.size() == 3);
}
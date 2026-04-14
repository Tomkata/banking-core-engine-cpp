#include "SqliteAccountRepository.h"

#include "../Factories/AccountFactory.h"

#include "../Database/SqliteStatementGuard.h"

#include "../../Domain/Models/Accounts/CheckingAccount.h"
#include "../../Domain/Models/Accounts/DepositAccount.h"
#include "../../Domain/Models/Accounts/SavingsAccount.h"


void SqliteAccountRepository::Add(const Account& account) {

	sqlite3_stmt* stmt;

	std::string sql = "INSERT INTO accounts (type,status,balance_cents) VALUES (?,?,?);";

	if (sqlite3_prepare_v2(db.GetConnection(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}

	SqliteStatementGuard guard{ stmt };

	
	//bind parameters
	sqlite3_bind_int(stmt, 1, static_cast<int>(account.GetType()));
	sqlite3_bind_int(stmt, 2, static_cast<int>(account.GetStatus()));
	sqlite3_bind_int64(stmt,3,account.GetBalance().GetCents());


	if (sqlite3_step(stmt) != SQLITE_DONE) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}

	if (account.GetType() == AccountType::Deposit) {
		auto& deposit = static_cast<const DepositAccount&>(account);

		sqlite3_stmt* stmt2;

		std::string sql2 = "INSERT INTO deposit_accounts (account_id, months, interest_rate) VALUES (?, ?, ?)";

		if (sqlite3_prepare_v2(db.GetConnection(), sql2.c_str(), -1, &stmt2, nullptr)) {
			throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
		}

		SqliteStatementGuard guard2{ stmt2 };

		sqlite3_bind_int(stmt2, 1, account.GetId());
		sqlite3_bind_int(stmt2, 2, deposit.GetMonths());
		sqlite3_bind_double(stmt2, 3, deposit.GetInterestRate());

		if (sqlite3_step(stmt2) != SQLITE_DONE) {
			throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
		}
		
	}
		
};

std::unique_ptr<Account> SqliteAccountRepository::FindById(int id) {
	sqlite3_stmt* stmt;

	std::string sql = R"(
    SELECT a.id, a.type, a.status, a.balance_cents,
           d.months, d.interest_rate
    FROM accounts a
    LEFT JOIN deposit_accounts d ON a.id = d.account_id
    WHERE a.id = ?
)";

	if (sqlite3_prepare_v2(db.GetConnection(),sql.c_str(), -1,&stmt , nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}

	SqliteStatementGuard guard{ stmt };

	sqlite3_bind_int(stmt, 1, id);
	std::unique_ptr<Account> account;


	if (sqlite3_step(stmt) == SQLITE_ROW){
		int id = sqlite3_column_int(stmt, 0);
		int type = sqlite3_column_int(stmt, 1);
		int status = sqlite3_column_int(stmt, 2);
		long long balance = sqlite3_column_int64(stmt, 3);
		int months = sqlite3_column_int(stmt, 4);     
		double rate = sqlite3_column_double(stmt, 5);   

		account = AccountFactory::Create(
		static_cast<AccountType>(type),
		id,
		static_cast<AccountStatus>(status),
		Money(balance),
			months,
			rate
		);
	}


	if (!account) {
		throw std::runtime_error("Account not found: " + std::to_string(id));
	}

	return account;
}

void  SqliteAccountRepository::Update(const Account& account) {
	sqlite3_stmt* stmt;

	std::string sql = "UPDATE accounts SET status = ?, balance_cents = ? WHERE id = ?";

	if (sqlite3_prepare_v2(db.GetConnection(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}
	SqliteStatementGuard guard{ stmt };

	sqlite3_bind_int(stmt, 1, static_cast<int>(account.GetStatus()));
	sqlite3_bind_int64(stmt, 2, account.GetBalance().GetCents());
	sqlite3_bind_int(stmt, 3, account.GetId());


	if (sqlite3_step(stmt) != SQLITE_DONE) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}
}



bool SqliteAccountRepository::Exists(int id) const {
	sqlite3_stmt* stmt;

	std::string sql = R"(SELECT COUNT(*) FROM accounts WHERE id = ?;)";

	if (sqlite3_prepare_v2(db.GetConnection(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}
	SqliteStatementGuard guard{ stmt };

	 sqlite3_bind_int(stmt,1,id);

	if (sqlite3_step(stmt) == SQLITE_ROW) {
		return sqlite3_column_int(stmt, 0) > 0;
	}
	return false;
}
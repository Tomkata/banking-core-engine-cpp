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
	sqlite3_bind_int64(stmt, 3, account.GetBalance().GetCents());


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
	else if (account.GetType() == AccountType::Saving) {
		auto& saving = static_cast<const SavingsAccount&>(account);

		sqlite3_stmt* stmt2;

		std::string sql2 = "INSERT INTO saving_accounts (account_id,interest_rate,lastAccrualDate) VALUES (?, ?, ?)";

		if (sqlite3_prepare_v2(db.GetConnection(), sql2.c_str(), -1, &stmt2, nullptr)) {
			throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
		}

		SqliteStatementGuard guard2{ stmt2 };

		sqlite3_bind_int(stmt2, 1, saving.GetId());
		sqlite3_bind_double(stmt2, 2, saving.GetInterestRate());
		auto lastAccrual = std::chrono::system_clock::to_time_t(saving.GetLastAccrualDate());
		sqlite3_bind_int64(stmt2, 3, static_cast<long long>(lastAccrual));

		if (sqlite3_step(stmt2) != SQLITE_DONE) {
			throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
		}
	}
};

std::unique_ptr<Account> SqliteAccountRepository::FindById(int id) {
	sqlite3_stmt* stmt;

	std::string sql = R"(SELECT id, type, status, balance_cents FROM accounts WHERE id = ?)";



	if (sqlite3_prepare_v2(db.GetConnection(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}

	SqliteStatementGuard guard{ stmt };

	sqlite3_bind_int(stmt, 1, id);
	std::unique_ptr<Account> account;


	if (sqlite3_step(stmt) == SQLITE_ROW) {
		int id = sqlite3_column_int(stmt, 0);
		int type = sqlite3_column_int(stmt, 1);
		int status = sqlite3_column_int(stmt, 2);
		long long balance = sqlite3_column_int64(stmt, 3);

		if (static_cast<AccountType>(type) == AccountType::Checking) {
			return std::make_unique<CheckingAccount>(id, static_cast<AccountStatus>(status), Money(balance));
		}
		else if (static_cast<AccountType>(type) == AccountType::Saving) {
			auto [rate, lastAccrual] = FindSavingExtra(id);
			return std::make_unique<SavingsAccount>(id, static_cast<AccountStatus>(status), Money(balance), rate, lastAccrual);
		}
		else if (static_cast<AccountType>(type) == AccountType::Deposit) {
			auto [months, rate] = FindDepositExtra(id);
			return std::make_unique<DepositAccount>(id, static_cast<AccountStatus>(status), Money(balance), months, rate);
		}
		else {
			throw std::runtime_error("Unknown account type: " + std::to_string(type));
		}

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
		std::cout << sqlite3_errmsg(db.GetConnection()) << std::endl;
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}

	if (account.GetType() == AccountType::Saving) {
		auto& saving = static_cast<const SavingsAccount&>(account);
		sqlite3_stmt* stmt2;
		std::string sql2 = "UPDATE saving_accounts SET lastAccrualDate = ? WHERE account_id = ?";

		if (sqlite3_prepare_v2(db.GetConnection(), sql2.c_str(), -1, &stmt2, nullptr) != SQLITE_OK) {
			throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
		}

		SqliteStatementGuard guard{ stmt2 };

		auto lastAccrual = std::chrono::system_clock::to_time_t(saving.GetLastAccrualDate());
		sqlite3_bind_int64(stmt2, 1, static_cast<long long>(lastAccrual));
		sqlite3_bind_int(stmt2, 2, account.GetId());
		if (sqlite3_step(stmt2) != SQLITE_DONE) {
			throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
		}
	}

}



bool SqliteAccountRepository::Exists(int id) const {
	sqlite3_stmt* stmt;

	std::string sql = R"(SELECT COUNT(*) FROM accounts WHERE id = ?;)";

	if (sqlite3_prepare_v2(db.GetConnection(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}
	SqliteStatementGuard guard{ stmt };

	sqlite3_bind_int(stmt, 1, id);

	if (sqlite3_step(stmt) == SQLITE_ROW) {
		return sqlite3_column_int(stmt, 0) > 0;
	}
	return false;
}


std::pair<double, std::chrono::system_clock::time_point> SqliteAccountRepository::FindSavingExtra(int id) {
	sqlite3_stmt* stmt;
	std::string sql = R"(SELECT interest_rate, lastAccrualDate FROM saving_accounts WHERE account_id  = ?)";

	if (sqlite3_prepare_v2(db.GetConnection(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}
	SqliteStatementGuard guard{ stmt };
	sqlite3_bind_int(stmt, 1, id);

	double interestRate = 0.0;
	std::chrono::system_clock::time_point lastAccrualDate;

	if (sqlite3_step(stmt) == SQLITE_ROW) {
		interestRate = sqlite3_column_double(stmt, 0);
		long long lastAccrualTs = sqlite3_column_int64(stmt, 1);
		lastAccrualDate = std::chrono::system_clock::from_time_t(static_cast<time_t>(lastAccrualTs));
	}

	return { interestRate,lastAccrualDate };
}

std::pair<int, double> SqliteAccountRepository::FindDepositExtra(int id) {
	sqlite3_stmt* stmt;
	std::string sql = R"(SELECT months, interest_rate FROM deposit_accounts WHERE account_id Ч = ?)";

	if (sqlite3_prepare_v2(db.GetConnection(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}
	SqliteStatementGuard guard{ stmt };
	sqlite3_bind_int(stmt, 1, id);

	int months = 0;
	double interestRate = 0.0;

	if (sqlite3_step(stmt) == SQLITE_ROW) {
		months = sqlite3_column_int(stmt, 0);
		interestRate = sqlite3_column_double(stmt, 1);
	}

	return { months,interestRate };
}


std::vector<std::unique_ptr<SavingsAccount>> SqliteAccountRepository::FindAllSavings() {
	sqlite3_stmt* stmt;
	std::string sql = R"(SELECT accounts.id, type, status, balance_cents, s.interest_rate, s.lastAccrualDate
FROM accounts
INNER JOIN saving_accounts s ON accounts.id = s.account_id
WHERE accounts.type = 2)";


	if (sqlite3_prepare_v2(db.GetConnection(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}
	SqliteStatementGuard guard{ stmt };

	std::vector<std::unique_ptr<SavingsAccount>> accounts;

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		int id = sqlite3_column_int(stmt, 0);
		int type = sqlite3_column_int(stmt, 1);
		int status = sqlite3_column_int(stmt, 2);
		long long balance = sqlite3_column_int64(stmt, 3);
		double interestRate = sqlite3_column_double(stmt, 4);
		long long lastAccrualTs = sqlite3_column_int64(stmt, 5);
		std::chrono::system_clock::time_point lastAccrualDate = std::chrono::system_clock::from_time_t(static_cast<time_t>(lastAccrualTs));

		accounts.push_back(
			std::make_unique<SavingsAccount>(id, static_cast<AccountStatus>(status), Money(balance), interestRate, lastAccrualDate)
		);
	}


	return accounts;
}
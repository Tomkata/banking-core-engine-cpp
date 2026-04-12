#include "SqliteTransactionRepository.h"
#include "../Database/SqliteStatementGuard.h"

#include <iostream>

void SqliteTransactionRepository::Add( Transaction&& tr) {
	sqlite3_stmt* stmt;

	std::string sql = "INSERT INTO transactions (id,type,status,amount_cents,from_account_id,to_account_id,failure_reason) VALUES (?,?,?,?,?,?,?)";

	if (sqlite3_prepare_v2(db.GetConnection(),sql.c_str(),-1,&stmt,nullptr) != SQLITE_OK){
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}
	SqliteStatementGuard guard{ stmt };

	sqlite3_bind_int(stmt, 1, tr.GetId());
	sqlite3_bind_int(stmt, 2, static_cast<int>(tr.GetType()));
	sqlite3_bind_int(stmt, 3, static_cast<int>(tr.GetStatus()));
	sqlite3_bind_int64(stmt, 4,tr.GetAmount().GetCents());
	sqlite3_bind_int(stmt, 5, tr.GetFromAccountId());
	sqlite3_bind_int(stmt, 6, tr.GetToAccountId());
	sqlite3_bind_text(stmt, 7, tr.GetFailureReason().c_str(),-1, SQLITE_TRANSIENT);

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}

	for (const auto& entry : tr.GetEntries()) {
		sqlite3_stmt* stmt2;
		std::string sql2 = "INSERT INTO transaction_entries (transaction_id, entry_type, ledger_type, amount_cents, description) VALUES (?,?,?,?,?)";

		sqlite3_prepare_v2(db.GetConnection(), sql2.c_str(), -1, &stmt2, nullptr);
		SqliteStatementGuard guard2{ stmt2 };

		sqlite3_bind_int(stmt2, 1, tr.GetId());
		sqlite3_bind_int(stmt2, 2, static_cast<int>(entry.GetType()));
		sqlite3_bind_int(stmt2, 3, static_cast<int>(entry.GetLedgerType()));
		sqlite3_bind_int64(stmt2, 4, entry.GetAmount().GetCents());
		sqlite3_bind_text(stmt2, 5, entry.GetDescription().c_str(), -1, SQLITE_TRANSIENT);

		if (sqlite3_step(stmt2) != SQLITE_DONE) {
			throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
		}
	}
}

void SqliteTransactionRepository::Update(Transaction& transaction) {
	sqlite3_stmt* stmt;

	std::string sql = "UPDATE transactions SET status = ?, failure_reason = ? WHERE id = ?";

	if (sqlite3_prepare_v2(db.GetConnection(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}
	SqliteStatementGuard guard{ stmt };

	sqlite3_bind_int(stmt, 1, static_cast<int>(transaction.GetStatus()));
	sqlite3_bind_text(stmt, 2, transaction.GetFailureReason().c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, transaction.GetId());

	if (sqlite3_step(stmt) != SQLITE_DONE){
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}

}

std::unique_ptr<Transaction> SqliteTransactionRepository::FindById(int id) {
	sqlite3_stmt* stmt;

	std::string sql = "SELECT id,type,status,amount_cents,from_account_id,to_account_id,failure_reason FROM transactions WHERE id = ?";

	if (sqlite3_prepare_v2(db.GetConnection(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}

	SqliteStatementGuard guard{ stmt };

	sqlite3_bind_int(stmt, 1, id);
	std::unique_ptr<Transaction> tr;

	if (sqlite3_step(stmt) == SQLITE_ROW)
	{
		int id = sqlite3_column_int(stmt, 0);
		int type = sqlite3_column_int(stmt, 1);
		int status = sqlite3_column_int(stmt, 2);
		long long amount = sqlite3_column_int64(stmt, 3);
		int fromId = sqlite3_column_int(stmt, 4);
		int toId = sqlite3_column_int(stmt, 5);
		const char* reason = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));

		tr = std::make_unique<Transaction>(
			id,
			static_cast<TransactionType>(type),
			Money(amount),
			fromId,
			toId,
			static_cast<TransactionStatus>(status),
			reason ? std::string(reason) : ""
		);
	}


	if (!tr) {
		throw std::runtime_error("Account not found: " + std::to_string(id));
	}
	return tr;
};
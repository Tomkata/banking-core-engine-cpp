#include "AccountQueryService.h"
#include "../Database/SqliteStatementGuard.h"
#include "iostream"
#include "string"


AccountStatement AccountQueryService::GetStatement(int accountId) {
	sqlite3_stmt* stmt;

	std::string sql = R"(
SELECT accounts.id, accounts.type, accounts.balance_cents, tr.created_at, tr.amount_cents, trEn.entry_type, trEn.description 
FROM accounts
LEFT JOIN transactions tr ON tr.from_account_id = accounts.id OR tr.to_account_id = accounts.id
LEFT JOIN transaction_entries trEn ON tr.id = trEn.transaction_id
WHERE accounts.id = ?
)";

		if (sqlite3_prepare_v2(db.GetConnection(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db.GetConnection()));
	}
		SqliteStatementGuard guard{ stmt };

		sqlite3_bind_int(stmt,1,accountId);

		
		bool firstRow = true;
		AccountStatement statement;
		while (sqlite3_step(stmt) == SQLITE_ROW){
			if (firstRow){
				statement.AccountId = sqlite3_column_int(stmt, 0);
				statement.type = static_cast<AccountType>(sqlite3_column_int(stmt, 1));
				statement.balance = Money(sqlite3_column_int64(stmt, 2));
				firstRow = false;
			}
			if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) {
				StatementLine line;
				long long createdAt = sqlite3_column_int64(stmt, 3);
				line.CreatedAt = std::chrono::system_clock::from_time_t(static_cast<time_t>(createdAt));
				line.Amount = Money(sqlite3_column_int64(stmt, 4));
				line.trEntryType = static_cast<TransactionEntryType>(sqlite3_column_int(stmt, 5));
				line.Desciption = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
				statement.satementLines.push_back(line);
			}
		}
		return statement;
}	
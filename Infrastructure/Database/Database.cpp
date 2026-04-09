
#include "Database.h"
#include <exception>
#include <iostream>

void Database::Execute(const std::string& sql) {
	char* errMsg = nullptr;
	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::string error = errMsg;
		sqlite3_free(db);
		throw std::runtime_error(error);
	}
};

Database::Database(const std::string& filename) {
	if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
		throw std::runtime_error(sqlite3_errmsg(db));
}
	}

Database::~Database() {
	if (db) sqlite3_close(db);
}


void Database::BeginTransaction() {
	Execute("BEGIN TRANSACTION;");
}

void Database::Commit() {
	Execute("COMMIT;");
}

void Database::Rollback() {
	Execute("ROLLBACK;");
}
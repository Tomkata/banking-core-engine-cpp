#pragma once

#include <string>
#include "sqlite3.h"

class Database {

private:
	sqlite3* db;


public:
	Database(const std::string& filename);
	~Database();

	sqlite3* GetConnection();

	void Execute(const std::string& sql);

	void BeginTransaction();
	void Commit();
	void Rollback();
};

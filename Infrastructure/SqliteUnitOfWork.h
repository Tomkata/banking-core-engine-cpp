#pragma once
#include "../Domain/Interfaces/IUnitOfWork.h"
#include "../Infrastructure/Database/Database.h"

class SqliteUnitOfWork : public  IUnitOfWork{
	Database& db;

public:
	SqliteUnitOfWork(Database& db) : db(db) {}
	virtual void BeginTransaction() { db.BeginTransaction(); };
	virtual void Commit() { db.Commit(); };
	virtual void Rollback() { db.Rollback(); };
};	
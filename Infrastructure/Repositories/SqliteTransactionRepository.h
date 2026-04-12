#pragma once

#include "../../Domain/Models/Transaction.h"
#include "../../Domain/reposiories/ITransactionRepository.h"
#include "../Database/Database.h"

class SqliteTransactionRepository : public ITransactionRepository {
private:
	Database& db;

public:
	SqliteTransactionRepository(Database& db) :db(db) {}

	virtual void Add( Transaction&& tr) override;
	virtual void Update(Transaction& transaction) override;
	virtual std::unique_ptr<Transaction> FindById(int id) override;

};
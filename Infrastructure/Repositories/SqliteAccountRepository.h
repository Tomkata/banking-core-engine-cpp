#pragma once
#include "../../Domain/Models/Accounts/Account.h"
#include "../../Domain/reposiories/IAccountRepository.h"
#include "../Database/Database.h"

class SqliteAccountRepository : public IAccountRepository {

	private:
		Database& db;

public:
	SqliteAccountRepository(Database& db) :db(db){}

	void Add(const Account& account) override;
	virtual std::unique_ptr<Account> FindById(int id) override;
	virtual bool Exists(int id) const override;
	virtual void PrintAll() const override;
	virtual void Update(const Account& account) override;
};
	
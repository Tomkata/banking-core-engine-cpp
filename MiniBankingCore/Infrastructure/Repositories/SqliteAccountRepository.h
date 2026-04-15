#pragma once
#include "../../Domain/Models/Accounts/Account.h"
#include "../../Domain/interfaces/IAccountRepository.h"
#include "../Database/Database.h"
#include <chrono>

class SqliteAccountRepository : public IAccountRepository {

	private:
		Database& db;

public:
	SqliteAccountRepository(Database& db) :db(db){}

	virtual void Add(const Account& account) override;
	virtual std::unique_ptr<Account> FindById(int id) override;
	virtual bool Exists(int id) const override;
	virtual void Update(const Account& account) override;
	std::pair<double, std::chrono::system_clock::time_point> FindSavingExtra(int id);
	std::pair<int, double> FindDepositExtra(int id);
};
	
#pragma once

#include "../../MiniBankingCore/Domain/Interfaces/IAccountRepository.h"
#include "../../MiniBankingCore/Infrastructure/Factories/AccountFactory.h"
#include <unordered_map>
#include <chrono>
#include <iostream>


class FakeAccountRepository : public IAccountRepository {
private:
	int failOnUpdateId = -1;

private:
	struct AccountData {
		AccountType type;
		int id = 0;
		AccountStatus status;
		Money balance;
		int months = 0;
		double rate = 0.0;
		std::chrono::system_clock::time_point lastAccrualDate = std::chrono::system_clock::now();

	};

	std::unordered_map<int, AccountData> accounts;

public:
	void Seed(AccountType type, int id, AccountStatus status, Money balance,
		int months = 0, double rate = 0.0,
		std::chrono::system_clock::time_point lastAccrualDate = std::chrono::system_clock::now()) {
		accounts[id] = { type,id,status,balance,months,rate,lastAccrualDate };
	};	

	std::unique_ptr<Account> FindById(int id) override {
		auto it = accounts.find(id);

		if (it == accounts.end()) return nullptr;

		auto& d = it->second;
		return AccountFactory::Create(d.type, d.id, d.status, d.balance, d.months, d.rate,
			d.lastAccrualDate);
	}

	void Add(const Account& account) override {}

	bool Exists(int id) const override {
		return accounts.find(id) != accounts.end();
	}

	void Update(const Account& account) override {
		if (account.GetId() == failOnUpdateId) {
			throw std::runtime_error("Simulated update failure");
		}

		auto it = accounts.find(account.GetId());

		if (it != accounts.end()) {
			it->second.status = account.GetStatus();
			it->second.balance = account.GetBalance();
		}
		it->second.balance = account.GetBalance();
		std::cout << account.GetBalance().GetCents();
	}

public:
	void SetFailOnUpdate(int id) {
		failOnUpdateId = id;
	}

	std::vector<std::unique_ptr<SavingsAccount>> FindAllSavings() override {
    std::vector<std::unique_ptr<SavingsAccount>> result;
    for (auto& [id, d] : accounts) {
        if (d.type == AccountType::Saving) {
            result.push_back(std::make_unique<SavingsAccount>(
                d.id, d.status, d.balance, d.rate, d.lastAccrualDate
            ));
        }
    }
    return result;
}
};
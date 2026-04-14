#pragma once

#include "../../MiniBankingCore/Domain/Interfaces/IAccountRepository.h"
#include "../../MiniBankingCore/Infrastructure/Factories/AccountFactory.h"
#include <unordered_map>

class FakeAccountRepository : public IAccountRepository {
private:
    struct AccountData {
        AccountType type;
        int id;
        AccountStatus status;
        Money balance;
        int months;
        double rate;
    };

    std::unordered_map<int, AccountData> accounts;

public:
    void Seed(AccountType type, int id, AccountStatus status, Money balance,
        int months = 0, double rate = 0.0) {
        accounts[id] = { type,id,status,balance,months,rate };
    }

    std::unique_ptr<Account> FindById(int id) override {
        auto it = accounts.find(id);

        if (it == accounts.end()) return nullptr;

        auto& d = it->second;
        return AccountFactory::Create(d.type,d.id,d.status,d.balance,d.months,d.rate);
    }

    void Add(const Account& account) override {}

    bool Exists(int id) const override{
        return accounts.find(id) != accounts.end();
    }

    void Update(const Account& account) override {
        auto it = accounts.find(account.GetId());

        if (it != accounts.end()) {
            it->second.status = account.GetStatus();
            it->second.balance = account.GetBalance();

        }
    }
};
#pragma once
#include "../Models/Accounts/Account.h"
#include "../reposiories/IAccountRepository.h"

#include <vector>
#include <memory> 

class AccountRepository : public IAccountRepository{

private:
	std::vector<std::unique_ptr<Account>> accounts;

	public:
		void Add(std::unique_ptr<Account> account) override;
		Account* FindById(int id) override;
		bool Exists(int id) const override;
		void PrintAll() const override;
		std::string AccountStatusToString(AccountStatus status) const;

};
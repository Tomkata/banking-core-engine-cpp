#pragma once
#include "../Models/Accounts/Account.h"

#include <vector>
#include <memory> 

class AccountRepository {

private:
	std::vector<std::unique_ptr<Account>> accounts;

	public:
		void Add(std::unique_ptr<Account> account);
		Account* FindById(int id);
		bool Exists(int id) const;
		void PrintAll() const;
		std::string AccountStatusToString(AccountStatus status) const;

};
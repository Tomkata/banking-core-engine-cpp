#pragma once
#include "../Models/Accounts/Account.h"

#include <vector>
#include <memory> 

class IAccountRepository {
public:
	virtual ~IAccountRepository() = default;
	virtual void Add(std::unique_ptr<Account> account) = 0;
	virtual Account* FindById(int id) = 0;
	virtual bool Exists(int id) const = 0;
	virtual void PrintAll() const = 0;
};
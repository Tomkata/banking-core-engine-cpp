#pragma once
#include "../Models/Accounts/Account.h"

#include <vector>
#include <memory> 

class IAccountRepository {
	
public:
	virtual ~IAccountRepository() = default;
	virtual void Add(const Account& account) = 0;
	virtual std::unique_ptr<Account>  FindById(int id) = 0;
	virtual bool Exists(int id) const = 0;
	virtual void Update(const Account& account) = 0;
};
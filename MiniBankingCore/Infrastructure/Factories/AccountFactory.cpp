#include "AccountFactory.h"
#include "../../Domain/Models/Accounts/CheckingAccount.h"
#include "../../Domain/Models/Accounts/DepositAccount.h"
#include "../../Domain/Models/Accounts/SavingsAccount.h"

std::unique_ptr<Account> AccountFactory::Create(AccountType type,
	int id,
	AccountStatus status,
	Money balance,
	int months, 
	double rate,
	std::chrono::system_clock::time_point lastAccrualDate) {
	switch (type)
	{
	case AccountType::Checking: return std::make_unique<CheckingAccount>(id,status,balance);
	case AccountType::Deposit:  return std::make_unique<DepositAccount>(id, status, balance, months, rate);
	case AccountType::Saving:return std::make_unique<SavingsAccount>(id, status, balance,rate, lastAccrualDate);
	default:
		throw std::runtime_error("Invalid account type");	
		break;
	}
}
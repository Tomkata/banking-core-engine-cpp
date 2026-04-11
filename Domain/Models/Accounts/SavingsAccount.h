#pragma once
#include <iostream>
#include "../Accounts/Account.h"
#include "../../enums/WithdrawResult.h"

class SavingsAccount :public Account {

	
public:
	SavingsAccount()
	:Account(AccountType::Saving){}

	SavingsAccount(int id, AccountStatus status, Money balance)
		:Account(AccountType::Saving,id,status,balance){ }

	DepositResult CanDeposit(const Money& amount) const override {
		return Account::BaseCanDeposit(amount);
	}

	WithdrawResult CanWithdraw(const Money& amount) const override {
		return WithdrawResult::NotSupported;
	}


};
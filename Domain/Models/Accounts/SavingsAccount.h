#pragma once
#include <iostream>
#include "../Accounts/Account.h"
#include "../../enums/WithdrawResult.h"

class SavingsAccount :public Account {

	
public:
	SavingsAccount()
	:Account(AccountType::Saving){}

	DepositResult CanDeposit(const Money& amount) const override {
		return Account::BaseCanDeposit(amount);
	}

	WithdrawResult CanWithdraw(const Money& amount) const override {
		return WithdrawResult::NotSupported;
	}


};
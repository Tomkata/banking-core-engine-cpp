#pragma once
#include <iostream>
#include "../Accounts/Account.h"
#include "../../enums/WithdrawResult.h"
#include <chrono>
#include <iostream>

class SavingsAccount :public Account {
private:
	double interestRate;
	std::chrono::system_clock::time_point lastAccrualDate;
	
public:
	SavingsAccount()
		: Account(AccountType::Saving),
		interestRate(0.0),
		lastAccrualDate(std::chrono::system_clock::now()) {
	}

	SavingsAccount(int id,
		AccountStatus status,
		Money balance,
		double interestRate,
		std::chrono::system_clock::time_point lastAccrualDate)
		:Account(AccountType::Saving, id, status, balance),
		interestRate(interestRate),
		lastAccrualDate(lastAccrualDate) {
	}
	SavingsAccount(
		Money balance,
		double interestRate):
		Account(AccountType::Saving),
		interestRate(interestRate),
		lastAccrualDate(std::chrono::system_clock::now())	

	{
	}

	DepositResult CanDeposit(const Money& amount) const override {
		return Account::BaseCanDeposit(amount);
	}

	WithdrawResult CanWithdraw(const Money& amount) const override {
		return WithdrawResult::NotSupported;
	}

public:
	double GetInterestRate() const {
		return interestRate;
	}

	std::chrono::system_clock::time_point GetLastAccrualDate() const {
		return lastAccrualDate;
	}

	void UpdateLastAccrualDate() {
		lastAccrualDate = std::chrono::system_clock::now();
	}

};
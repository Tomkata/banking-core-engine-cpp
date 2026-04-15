#pragma once

#include "../../Domain/Models/Accounts/Account.h"
#include "../../Domain/ValueObjects/Money.h"

#include <iostream>
#include <chrono>

class AccountFactory {

public:
	static std::unique_ptr<Account> Create(
		AccountType type,
		int id,
		AccountStatus status,
		Money balance,
		int months,
		double rate,
		std::chrono::system_clock::time_point lastAccrualDate);
};
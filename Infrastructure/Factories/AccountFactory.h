#pragma once

#include "../../Domain/Models/Accounts/Account.h"
#include "../../Domain/ValueObjects/Money.h"

#include <iostream>


class AccountFactory {

public:
	static std::unique_ptr<Account> Create(
		AccountType type,
		int id,
		AccountStatus status,
		Money balance,
		int months,
		double rate
	);
};
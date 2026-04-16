#pragma once

#include <iostream>
#include <vector>

#include "StatementLine.h"
#include "../enums/AccountType.h"
#include "../ValueObjects/Money.h"


struct AccountStatement {
	int AccountId;
	AccountType type;
	Money balance;
	std::vector<StatementLine> satementLines;
};
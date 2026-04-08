#pragma once

#include <iostream>
#include <vector>


#include "../Effects/Effect.h"
#include "../Accounts/Account.h"
#include "../../ValueObjects/Money.h"

 class TransferOperation {

 public:
	std::vector<Effect> CreateEffects(
		Account& accountA,
		Account& accountB,
		Money amount
	);
};
#pragma once

#include <iostream>
#include <vector>


#include "../Effects/Effect.h"
#include "../Accounts/Account.h"
#include "../../ValueObjects/Money.h"
#include "../services/TransferFeeCalculator.h"
#include "../../../Domain/BankConstants.h"
#include "../../Results/Result.h"
#include "../../enums/TransferResult.h"

 class TransferOperation {

 public:
	 Result<std::vector<Effect>, TransferResult> CreateEffects(
		Account& accountA,
		Account& accountB,
		Money amount,
		Money& fee
	);
};
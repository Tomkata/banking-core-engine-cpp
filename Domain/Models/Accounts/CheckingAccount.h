#pragma once
#include "../Accounts/Account.h"
#include "../../enums/WithdrawResult.h"

class CheckingAccount : public Account {
public:
    CheckingAccount()
        : Account(AccountType::Checking) {
    }

    WithdrawResult  CanWithdraw(const Money& amount) const override ;
    DepositResult CanDeposit(const Money& amount) const override;
};
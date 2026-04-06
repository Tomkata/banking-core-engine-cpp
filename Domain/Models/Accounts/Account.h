#pragma once

#include "../../enums/AccountType.h"
#include "../../enums/AccountStatus.h"
#include "../../enums/WithdrawResult.h"
#include "../../enums/DepositResult.h"
#include "../../enums/WithdrawCapability.h"
#include "../Effects//Effect.h"
#include "../../ValueObjects/Money.h"

#include <string>
#include <iostream>
#include <vector>

class Account {

private:
    static int nextId;
    int id;
    AccountType type;
    AccountStatus status;
    Money balance;


public:
    Account(AccountType type);

    virtual ~Account() = default;

   virtual std::vector<Effect> Deposit(const Money& amount);
   virtual std::vector<Effect> Withdraw(const Money& amount);
    void Freeze();
    void Close();
    void Block();
    int GetId() const;
    const Money& GetBalance() const;
    AccountStatus GetStatus() const;
    void ApplyDelta(const Money& delta);


protected:
    WithdrawResult BaseCanWithdraw(const Money& amount) const;
    DepositResult BaseCanDeposit(const Money& amount) const;
    virtual Money GetPenalty(const Money& amount) const { return Money(0); }

public:
    virtual WithdrawResult  CanWithdraw(const Money& amount) const = 0;
    virtual DepositResult CanDeposit(const Money& amount) const = 0;

    


}; 
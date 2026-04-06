#include "CheckingAccount.h"

WithdrawResult  CheckingAccount::CanWithdraw(const Money& amount)  const  {
    return BaseCanWithdraw(amount);
}       

DepositResult CheckingAccount::CanDeposit(const Money& amount) const {
    return BaseCanDeposit(amount);
}
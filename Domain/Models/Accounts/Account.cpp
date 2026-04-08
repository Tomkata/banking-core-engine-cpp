#include "../Accounts/Account.h"
#include "../../enums/AccountType.h"
#include "../../enums/TargetType.h"
#include "../../enums/EffectReason.h"
#include "../../enums/AccountStatus.h"
#include "../../Exceptions/Account/InvalidDepositException.h"
#include "../../Exceptions/Account/InsufficientBalanceException.h"
#include "../../Exceptions/Account/InvalidWithdrawException.h"

#include <iostream>
#include <vector>

static const int vaultId = 100;

int Account::nextId = 1;

Account::Account(AccountType type)
	: id(nextId++), type(type), balance(0), status(AccountStatus::Active) {
}



std::vector<Effect> Account::Deposit(const Money& amount) {
	DepositResult result = BaseCanDeposit(amount);
	if (result != DepositResult::Ok) {
		throw InvalidDepositException(result);
	}

	return {
		Effect(EffectTarget{TargetType::CustomerAccount, id},
			amount,
			EffectReason::Deposit),
		Effect(EffectTarget{TargetType::Vault, vaultId},
			amount,
			EffectReason::Deposit)
	};
}

std::vector<Effect> Account::Withdraw(const Money& amount) {
	auto result = CanWithdraw(amount);
	if (result != WithdrawResult::Ok) {
		throw InvalidWithdrawException(result);
	}


	return {
		Effect(EffectTarget{TargetType::CustomerAccount, id},
			-amount,
			EffectReason::Withdraw),
		Effect(EffectTarget{TargetType::Vault, vaultId},
			-amount,
			EffectReason::Withdraw)
	};
}


DepositResult Account::BaseCanDeposit(const Money& amount) const {
	if (status == AccountStatus::Closed) return DepositResult::Closed;
	if (status == AccountStatus::Blocked) return DepositResult::Blocked;

	return DepositResult::Ok;
}


WithdrawResult Account::BaseCanWithdraw(const Money& amount) const {

	if (amount <= Money(0))
		return WithdrawResult::InvalidAmount;

	if (status == AccountStatus::Frozen) return WithdrawResult::Frozen;
	if (status == AccountStatus::Closed) return WithdrawResult::Closed;
	if (status == AccountStatus::Blocked) return WithdrawResult::Blocked;
	if (balance < amount) return WithdrawResult::InsufficientFunds;

	return WithdrawResult::Ok;
}

int Account::GetId() const {
	return id;
}

const Money& Account::GetBalance() const {
	return balance;
}

AccountStatus Account::GetStatus() const {
	return status;
}


void Account::Close() {
	if (balance > Money(0))
		throw std::logic_error("Cannot close account with positive balance");
	if (status == AccountStatus::Closed)
		throw std::logic_error("Account is already closed");
	status = AccountStatus::Closed;
}

void Account::Freeze() {
	if (status == AccountStatus::Closed)
		throw std::logic_error("Cannot freeze a closed account");
	status = AccountStatus::Frozen;
}

void Account::Block() {
	if (status == AccountStatus::Closed)
		throw std::logic_error("Cannot block a closed account");
	status = AccountStatus::Blocked;
}

void Account::ApplyDelta(const Money& delta) {
	

	balance +=  delta;
}	

#include "TransferOperation.h"
#include "../../Exceptions/Account/InvalidWithdrawException.h"
#include "../../Exceptions/Account/InvalidDepositException.h"

std::vector<Effect> TransferOperation::CreateEffects(
	Account& accountA,
	Account& accountB,
	Money amount) {

	auto accountAResult = accountA.CanWithdraw(amount);
	auto accountBResult = accountB.CanDeposit(amount);
	if (accountA.GetId() == accountB.GetId()) {
		throw std::logic_error("Cannot transfer to the same account");
	}

	if (accountAResult != WithdrawResult::Ok) {
		throw InvalidWithdrawException(accountAResult);
	}
	if (accountBResult != DepositResult::Ok) {
		throw InvalidDepositException(accountBResult);
	}

	return {
		Effect(EffectTarget{TargetType::CustomerAccount,accountA.GetId()},-amount,EffectReason::Transfer),
		Effect(EffectTarget{TargetType::CustomerAccount,accountB.GetId()},amount,EffectReason::Transfer),
	};
};
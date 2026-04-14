
#include "TransferOperation.h"
#include "../../Exceptions/Account/InvalidWithdrawException.h"
#include "../../Exceptions/Account/InvalidDepositException.h"

 std::vector<Effect> TransferOperation::CreateEffects(
	Account& accountA,
	Account& accountB,
	Money amount,
	 Money& fee) {

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

	if (fee == Money(0)) {
		return {
		Effect(EffectTarget{TargetType::CustomerAccount,accountA.GetId()},-amount,EffectReason::Transfer),
		Effect(EffectTarget{TargetType::CustomerAccount,accountB.GetId()},amount,EffectReason::Transfer),
		};
	}

	return {
		Effect(EffectTarget{TargetType::CustomerAccount,accountA.GetId()},-amount,EffectReason::Transfer),
		Effect(EffectTarget{TargetType::CustomerAccount,accountA.GetId()},-fee,EffectReason::TransferFee),
		Effect(EffectTarget{TargetType::BankRevenue, BankContracts::BankRevenueId},fee,EffectReason::TransferFee),
		Effect(EffectTarget{TargetType::CustomerAccount,accountB.GetId()},amount,EffectReason::Transfer),
	};
	
};
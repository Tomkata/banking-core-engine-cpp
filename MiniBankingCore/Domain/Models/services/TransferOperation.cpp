
#include "TransferOperation.h"
#include "../../Exceptions/Account/InvalidWithdrawException.h"
#include "../../Exceptions/Account/InvalidDepositException.h"

Result<std::vector<Effect>, TransferResult> TransferOperation::CreateEffects(
	Account& accountA,
	Account& accountB,
	Money amount,
	 Money& fee) {

	auto accountAResult = accountA.CanWithdraw(amount);
	auto accountBResult = accountB.CanDeposit(amount);
	if (accountA.GetId() == accountB.GetId()) {
		return Result<std::vector<Effect>, TransferResult>::Err(TransferResult::SameAccount);
	}
		
	if (accountAResult != WithdrawResult::Ok) {
		return Result<std::vector<Effect>, TransferResult>::Err(FromWithdrawResult(accountAResult));
	}
	if (accountBResult != DepositResult::Ok) {
		return Result<std::vector<Effect>, TransferResult>::Err(FromDepositResult(accountBResult));
	}

	if (fee == Money(0)) {
		return Result< std::vector<Effect>, TransferResult>::Ok({
		Effect(EffectTarget{TargetType::CustomerAccount,accountA.GetId()},-amount,EffectReason::Transfer),
		Effect(EffectTarget{TargetType::CustomerAccount,accountB.GetId()},amount,EffectReason::Transfer),
		});
	}

	return Result< std::vector<Effect>, TransferResult>::Ok({
		Effect(EffectTarget{TargetType::CustomerAccount,accountA.GetId()},-amount,EffectReason::Transfer),
		Effect(EffectTarget{TargetType::CustomerAccount,accountA.GetId()},-fee,EffectReason::TransferFee),
		Effect(EffectTarget{TargetType::BankRevenue, BankContracts::BankRevenueId},fee,EffectReason::TransferFee),
		Effect(EffectTarget{TargetType::CustomerAccount,accountB.GetId()},amount,EffectReason::Transfer),
	});
	
};
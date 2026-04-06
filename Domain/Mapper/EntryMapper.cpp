#include "EntryMapper.h"
#include "../enums/TargetType.h"
#include  <cmath>


std::vector<TransactionEntry> EntryMapper::Map(const Effect& effect) {
	auto accountType = GetTargetType(effect.target.type);
	auto entryType = GetEntryType(accountType, effect.delta);

	return {
		TransactionEntry(entryType, accountType, effect.delta.Abs(), EffectReasonToString(effect.reason))
	};
}

LedgerAccountType EntryMapper::GetTargetType(TargetType type) {
	switch (type) {
	case TargetType::CustomerAccount: return LedgerAccountType::Liability;
	case TargetType::Vault: return LedgerAccountType::Asset;
	case TargetType::BankRevenue: return LedgerAccountType::Revenue;
	default: throw std::logic_error("Unknown target type");
	}
}

TransactionEntryType EntryMapper::GetEntryType(LedgerAccountType accountType, const Money& delta) {
	switch (accountType)
	{
	case Asset: return delta < Money(0) ? TransactionEntryType::Credit : TransactionEntryType::Debit;
	case Liability: return delta < Money(0) ? TransactionEntryType::Debit : TransactionEntryType::Credit;
	case Revenue: return delta < Money(0) ? TransactionEntryType::Debit : TransactionEntryType::Credit;
	default: throw std::logic_error("Unknown account type");
	}
}





 
 std::string EntryMapper::EffectReasonToString(EffectReason reason) {
	switch (reason) {
	case EffectReason::Withdraw:                return "Withdraw";
	case EffectReason::Deposit:                 return "Deposit";
	case EffectReason::Transfer:                return "Transfer";
	case EffectReason::EarlyWithdrawalPenalty:  return "Early Withdrawal Penalty";
	case EffectReason::TransferFee:             return "Transfer Fee";
	case EffectReason::MonthlyMaintenanceFee:   return "Monthly Maintenance Fee";
	case EffectReason::Tax:                     return "Tax";
	default:                                    return "Unknown";
	}
}
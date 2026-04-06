#pragma once

#include <iostream>
#include <vector>

#include "../Models/TransactionEntry.h"
#include "../Models/Effects/Effect.h"
#include "../enums/LedgerAccountType.h"

class EntryMapper {
public:
	std::vector<TransactionEntry> Map(const Effect& effect);
	LedgerAccountType GetTargetType(TargetType type);
	TransactionEntryType GetEntryType(LedgerAccountType accountType, const Money& delta);
	static std::string EffectReasonToString(EffectReason reason);

};
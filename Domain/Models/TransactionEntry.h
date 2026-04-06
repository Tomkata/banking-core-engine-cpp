#pragma once
#include "../enums/TransactionEntryType.h"
#include "../enums/LedgerAccountType.h"
#include "../ValueObjects/Money.h"

#include <string>
class TransactionEntry {

private:
	int id;
	static int nextId;
	TransactionEntryType entryType;
	LedgerAccountType ledgerType;
	Money amount;
	std::string description;

public:
	TransactionEntry(
		TransactionEntryType entryType,
		LedgerAccountType ledger,
		Money amount,
		const std::string description);

public:
	int GetId() const;
	TransactionEntryType GetType() const;
	LedgerAccountType GetLedgerType() const;
	const Money& GetAmount() const;
	std::string GetDescription() const;
};
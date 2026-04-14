#include "TransactionEntry.h"

int TransactionEntry::nextId = 1;

TransactionEntry::TransactionEntry(
    TransactionEntryType entryType,
    LedgerAccountType ledger,
    const Money amount,
    const std::string description)
    : id(nextId++),
    entryType(entryType),
    ledgerType(ledger),
    amount(amount),
    description(description) {
}

int TransactionEntry::GetId() const {
    return id;
}

const Money& TransactionEntry::GetAmount() const {
    return amount;
}
TransactionEntryType TransactionEntry::GetType() const {
    return entryType;
}

std::string TransactionEntry::GetDescription() const {
    return description;
}
LedgerAccountType TransactionEntry::GetLedgerType() const{
    return ledgerType;
}

    
#include "Transaction.h"
#include <stdexcept>

int Transaction::nextId = 1;


Transaction::Transaction(TransactionType type, Money amount, int fromAccountId, int toAccountId)
	: id(nextId++),
	type(type),
	amount(amount),
	fromAccountId(fromAccountId),
	toAccountId(toAccountId),
	status(TransactionStatus::Pending)
{}

void Transaction::MarkSuccess() {
	EnsureNotFinalized();
	if (!isBalanced()) {
		throw std::logic_error("Transaction is not balanced");
	}
	status = TransactionStatus::Success;
}


void Transaction::MarkFailed(const std::string &reason) {
	EnsureNotFinalized();
	status = TransactionStatus::Failed;
	failureReason = reason;
}

TransactionStatus Transaction::GetStatus() const {
	return status;
}

void Transaction::EnsureNotFinalized() const {
	if (status == Failed || status == Success) {
		throw std::logic_error("Transaction already finalized"); 
	}
}

const Money& Transaction::GetAmount() const {
	return amount;
}

int Transaction::GetFromAccountId() const {
	return fromAccountId;
}

int Transaction::GetToAccountId() const {
	return toAccountId;
}

TransactionType Transaction::GetType() const {
	return type;
}

const std::string& Transaction::GetFailureReason() const {
	return failureReason;
}

int Transaction::GetId() const {
	return id;
}



void Transaction::AddEntry(TransactionEntry&& entry) {
	entries.push_back(std::move(entry));
}

bool Transaction::isBalanced() const {
	Money sumDebit = Money(0);
	Money sumCredit = Money(0);

	for (auto& entry : entries) {
		if (entry.GetType() == TransactionEntryType::Credit) {
			sumCredit += entry.GetAmount();
	  }
		else sumDebit += entry.GetAmount();
	}

	return sumCredit == sumDebit;
}

const std::vector<TransactionEntry>& Transaction::GetEntries() const {
	return entries;
}
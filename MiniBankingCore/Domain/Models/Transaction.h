#pragma once
#include <vector>
#include <string>


#include "../enums/TransactionStatus.h"
#include "../ValueObjects/Money.h" 
#include "../enums/TransactionType.h"
#include "../Models/TransactionEntry.h"

class Transaction {

private:
	static int nextId;
	int id;
	std::vector<TransactionEntry> entries;
	Money amount;
	TransactionType type;
	TransactionStatus status;
	int fromAccountId;
	int toAccountId;
	std::string failureReason;

public:
	Transaction(TransactionType type, Money amount, int fromAccountId, int toAccountId);
	Transaction(int id, TransactionType type, Money amount, int fromAccountId, int toAccountId, TransactionStatus status, const std::string& failureReason);

public:
	void MarkSuccess();
	void MarkFailed(const std::string& reason);
	TransactionStatus GetStatus() const;
	const Money& GetAmount() const;
	int GetToAccountId() const;
	int GetFromAccountId() const;
	TransactionType GetType() const;
	int GetId() const;
	const std::string& GetFailureReason() const;
	void AddEntry(TransactionEntry&& entry);
	const std::vector<TransactionEntry>& GetEntries() const;

private:	
	void EnsureNotFinalized() const;
	bool isBalanced() const;

};
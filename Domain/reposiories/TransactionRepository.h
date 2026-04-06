#pragma once
#include "../Models/Transaction.h"

#include <vector>
#include <iostream>
class TransactionRepository {
private:
	std::vector<Transaction> transactions;

public:
	void Add(Transaction&& transaction);
	void PrintAll() const;

private:
	std::string TransactionTypeToString(TransactionType type) const;
	std::string TransactionStatusToString(TransactionStatus status) const;

};
#pragma once
#include "../Models/Transaction.h"

#include "../reposiories/ITransactionRepository.h"

#include <vector>
#include <iostream>
class TransactionRepository : public ITransactionRepository {
private:
	std::vector<Transaction> transactions;

public:
	void Add(Transaction&& transaction) override;
	void PrintAll() const override;

private:
	std::string TransactionTypeToString(TransactionType type) const;
	std::string TransactionStatusToString(TransactionStatus status) const;

};
#pragma once

#include "../Models/Transaction.h"

class ITransactionRepository {
public:
	virtual ~ITransactionRepository() = default;
	virtual void Add( Transaction&& transaction) = 0;
	virtual void Update(Transaction& transaction) = 0;
	virtual std::unique_ptr<Transaction> FindById(int id) = 0;

};		
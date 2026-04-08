#pragma once

#include "../Models/Transaction.h"

class ITransactionRepository {
public:
	virtual ~ITransactionRepository() = default;
	virtual void Add(Transaction&& transaction) = 0;
	virtual void PrintAll() const = 0;
};
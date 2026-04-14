#pragma once

#include "../../MiniBankingCore/Domain/Interfaces/ITransactionRepository.h"
#include <vector>

class FakeTransactionRepository : public ITransactionRepository {
private:
	std::vector<Transaction> transactions;


public:
	void Add(Transaction&& transaction) override {
		transactions.push_back(std::move(transaction));
	}

	void Update(Transaction& transaction) override {}

	std::unique_ptr<Transaction> FindById(int id) override {
		return nullptr;
	}

	const std::vector<Transaction>& GetAll() const {
		return transactions;
	}
};
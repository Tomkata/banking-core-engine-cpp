#pragma once


#include "../Models/Transaction.h"
#include "../reposiories/TransactionRepository.h"
#include "../reposiories/AccountRepository.h"
#include "../Mapper/EntryMapper.h"


class TransactionProcessor {
private:
	AccountRepository& accountRepo;
	TransactionRepository& transactionRepo;
	EntryMapper& mapper;

public:
	TransactionProcessor(AccountRepository& accountRepo, 
						TransactionRepository& transactionRepo,
						 EntryMapper& mapper)
		:
		accountRepo(accountRepo),	
		transactionRepo(transactionRepo),
		mapper(mapper){
	}
	void Transfer(int fromAccountId, int toAccountId, Money amount);
	void Withdraw(int fromAccountId, Money amount);
	void Deposit(int toAccountId, Money amount);

private:
	template<typename Func>
	std::vector<Effect> ExecuteTransaction(Transaction& tr, Func action);
	void StoreEntries( const std::vector<Effect>& effects, Transaction& tr);
	void ApplyEffects(const std::vector<Effect>& effects);
	void RollbackChanges(const std::vector<std::pair<Account*, Money>>& rollbackLog);
};

#pragma once


#include "../Models/Transaction.h"
#include "../interfaces/ITransactionRepository.h"
#include "../interfaces/IAccountRepository.h"
#include "../Mapper/EntryMapper.h"
#include "../Interfaces/IUnitOfWork.h"
#include "../Models/services/TransferFeeCalculator.h"
#include "../../Domain/Models/services/TransferOperation.h"

class TransactionProcessor {
private:
	IAccountRepository& accountRepo;
	ITransactionRepository& transactionRepo;
	EntryMapper& mapper;
	IUnitOfWork& uow;
	TransferFeeCalculator& calc;
	TransferOperation& transferOperation;

public:
	TransactionProcessor(IAccountRepository& accountRepo,
						ITransactionRepository& transactionRepo,
						 EntryMapper& mapper,
						 IUnitOfWork& uow,
						 TransferFeeCalculator& calc,
						TransferOperation& transferOperation)
		:
		accountRepo(accountRepo),	
		transactionRepo(transactionRepo),	
		mapper(mapper),
		uow(uow),
		calc(calc),
		transferOperation(transferOperation){
	}
	void Transfer(int fromAccountId, int toAccountId, Money amount);
	void Withdraw(int fromAccountId, Money amount);
	void Deposit(int toAccountId, Money amount);

private:
	template<typename Func>
	std::vector<Effect> ExecuteTransaction(Transaction& tr, Func action);
	void StoreEntries( const std::vector<Effect>& effects, Transaction& tr);
	void ApplyEffects(const std::vector<Effect>& effects);
};

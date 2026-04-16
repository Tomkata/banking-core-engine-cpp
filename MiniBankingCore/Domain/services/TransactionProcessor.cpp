#include "TransactionProcessor.h"
#include "../Interfaces/ITransactionRepository.h"

#include "../Models/services/TransferOperation.h"

#include "../enums/WithdrawResult.h"

#include "../Exceptions/Account/InvalidWithdrawException.h"
#include "../Exceptions/Account/InvalidDepositException.h"
#include "../Exceptions/Account/InvalidTransferException.h"



#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <unordered_map>



void TransactionProcessor::Deposit(int toAccountId, Money amount) {

	Transaction tr(TransactionType::Deposit, amount, -1, toAccountId);

	auto result  = ExecuteTransaction(tr, [&]() {

		auto toAccount = accountRepo.FindById(toAccountId);

		if (toAccount == nullptr) {
			throw std::logic_error("Account not found: id=" + std::to_string(toAccountId));
		}


		auto results = toAccount->Deposit(amount);
	
		if (!results.IsOk()) {
			throw InvalidDepositException(results.Error());
		}

		StoreEntries(results.Value(), tr);
		return results.Value();
		});

}


void TransactionProcessor::Transfer(int fromAccountId, int toAccountId, Money amount) {

	Transaction tr(TransactionType::Transfer, amount, fromAccountId, toAccountId);

	auto effects = ExecuteTransaction(tr, [&]() {
		auto fromAccount = accountRepo.FindById(fromAccountId);
		auto toAccount = accountRepo.FindById(toAccountId);
		if (fromAccount == nullptr) {
			throw std::logic_error("Account not found: id=" + std::to_string(fromAccountId));
		}
		if (toAccount == nullptr) {
			throw std::logic_error("Account not found: id=" + std::to_string(toAccountId));
		}

	
		auto fee = calc.CalculateFee(0.00,amount);

		auto effects = transferOperation.CreateEffects(*fromAccount,*toAccount,amount,fee);

		if (!effects.IsOk()) {
			throw InvalidTransferException(effects.Error());
		}

		StoreEntries(effects.Value(), tr);
		return effects.Value();
			
		});
}


void TransactionProcessor::Withdraw(int fromAccountId, Money amount) {

	Transaction tr(TransactionType::Withdraw, amount, fromAccountId, -1);
	
	auto effects = ExecuteTransaction(tr, [&]() {

		auto fromAccount = accountRepo.FindById(fromAccountId);

		if (fromAccount == nullptr) {
			throw std::logic_error("Account not found: id=" + std::to_string(fromAccountId));
		}

		auto results = fromAccount->Withdraw(amount);

		if (!results.IsOk()) {
			throw InvalidWithdrawException(results.Error());
		}

		
		StoreEntries(results.Value(), tr);
		return results.Value();
		});

}


template<typename Func>
std::vector<Effect> TransactionProcessor::ExecuteTransaction(Transaction& tr, Func action) {
	std::exception_ptr eptr = nullptr;
	std::vector<Effect> effects;
	uow.BeginTransaction();
	try
	{
		effects = action();
		if (tr.GetStatus() == Pending)
		{
			tr.MarkSuccess();
		}
		ApplyEffects(effects);
		transactionRepo.Add(std::move(tr));
		uow.Commit();
	}
	catch (const std::exception& ex)
	{
		tr.MarkFailed(ex.what());
		uow.Rollback();
		eptr = std::current_exception();
	}
	
	if (eptr)
		std::rethrow_exception(eptr);

	return effects;
}

void TransactionProcessor::StoreEntries(const std::vector<Effect>& effects, Transaction& tr) {

	for (auto& effect : effects) {
		auto  entries = mapper.Map(effect);
		for (auto&& entry : entries) {
			tr.AddEntry(std::move(entry));
		}
	}
}

void TransactionProcessor::ApplyEffects(const std::vector<Effect>& effects) {

	//prepare
	std::unordered_map<int, Money> deltas;
	std::unordered_map<int, std::unique_ptr<Account>> loadedAccounts;
	
	for (const auto& effect : effects) {
		if (effect.target.type != TargetType::CustomerAccount) 
			continue;
		int accountId = effect.target.targetId;
		if (loadedAccounts.find(accountId) == loadedAccounts.end()) {
			loadedAccounts[accountId] = accountRepo.FindById(accountId);
		}

		deltas[accountId] += effect.delta;
	}

	//validation
	for (const auto& [accountId,delta] : deltas) {

		if (!loadedAccounts[accountId]) {
			throw std::logic_error("Account not found: id=" + std::to_string(accountId));
		}

		if (loadedAccounts[accountId]->GetBalance() + delta < Money(0)) {
			throw std::logic_error("Invalid balance after apply: id=" + std::to_string(accountId));
		}
	}

	//commit

		for (const auto& [accountId, delta] : deltas) {
			if (delta == Money(0))
				continue;

			auto& acc = loadedAccounts[accountId];

			if (!acc)
				throw std::logic_error("Account not found during commit");

			acc->ApplyDelta(delta);
		}

	for (auto& [accountId, acc] : loadedAccounts) {

		accountRepo.Update(*acc);
		std::cout << "Updating account " << acc->GetId()
			<< " new balance: " << acc->GetBalance().GetCents() << "\n";
	}
	
}



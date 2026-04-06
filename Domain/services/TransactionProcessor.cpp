#include "TransactionProcessor.h"
#include "../reposiories/AccountRepository.h"
#include "../reposiories/TransactionRepository.h"

#include "../enums/WithdrawResult.h"

#include "../Exceptions/Account/InvalidWithdrawException.h"
#include "../Exceptions/Account/InvalidDepositException.h"



#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <unordered_map>



void TransactionProcessor::Deposit(int toAccountId, Money amount) {

	Transaction tr(TransactionType::Deposit, amount, -1, toAccountId);

	auto effects  = ExecuteTransaction(tr, [&]() {

		Account* toAccount = accountRepo.FindById(toAccountId);

		if (toAccount == nullptr) {
			throw std::logic_error("Account not found: id=" + std::to_string(toAccountId));
		}

		DepositResult result = toAccount->CanDeposit(amount);
		if (result != DepositResult::Ok) {
			throw InvalidDepositException(result);
		}

		auto effects = toAccount->Deposit(amount);
		//O(N * 2) = O(N)
		StoreEntries(effects, tr);
		return effects;
		});

}


void TransactionProcessor::Transfer(int fromAccountId, int toAccountId, Money amount) {

	Transaction tr(TransactionType::Transfer, amount, fromAccountId, toAccountId);

	auto effects = ExecuteTransaction(tr, [&]() {
		Account* fromAccount = accountRepo.FindById(fromAccountId);
		Account* toAccount = accountRepo.FindById(toAccountId);
		if (fromAccount == nullptr) {
			throw std::logic_error("Account not found: id=" + std::to_string(fromAccountId));
		}
		if (toAccount == nullptr) {
			throw std::logic_error("Account not found: id=" + std::to_string(toAccountId));
		}

		if (fromAccountId == toAccountId) {
			throw std::logic_error("Cannot transfer to the same account");
		}

		auto withdrawResult = fromAccount->CanWithdraw(amount);
		if (withdrawResult != WithdrawResult::Ok) {
			throw InvalidWithdrawException(withdrawResult);
		}

		DepositResult depositResult = toAccount->CanDeposit(amount);
		if (depositResult != DepositResult::Ok) {
			throw InvalidDepositException(depositResult);
		}

		std::vector<Effect> withdrawEffects = fromAccount->Withdraw(amount);
		std::vector<Effect> depositEffects = toAccount->Deposit(amount);

		std::vector<Effect> allEffects;
		allEffects.reserve(withdrawEffects.size() + depositEffects.size());

		allEffects.insert(allEffects.end(),withdrawEffects.begin(),withdrawEffects.end());
		allEffects.insert(allEffects.end(), depositEffects.begin(), depositEffects.end());

		StoreEntries(allEffects, tr);
		return allEffects;

		});
}


void TransactionProcessor::Withdraw(int fromAccountId, Money amount) {

	Transaction tr(TransactionType::Withdraw, amount, fromAccountId, -1);
	
	auto effects = ExecuteTransaction(tr, [&]() {

		Account* fromAccount = accountRepo.FindById(fromAccountId);

		if (fromAccount == nullptr) {
			throw std::logic_error("Account not found: id=" + std::to_string(fromAccountId));
		}

		auto result = fromAccount->CanWithdraw(amount);


		if (result != WithdrawResult::Ok) {
			throw InvalidWithdrawException(result);
		}

		std::vector<Effect> effects = fromAccount->Withdraw(amount);

		
		StoreEntries(effects,tr);
		return effects;

		});

}


template<typename Func>
std::vector<Effect> TransactionProcessor::ExecuteTransaction(Transaction& tr, Func action) {
	std::exception_ptr eptr = nullptr;
	std::vector<Effect> effects;
	try
	{
		effects = action();
		if (tr.GetStatus() == Pending)
			tr.MarkSuccess();
	}
	catch (const std::exception& ex)
	{
		tr.MarkFailed(ex.what());
		eptr = std::current_exception();
	}
	ApplyEffects(effects);
	transactionRepo.Add(std::move(tr));

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
	
	for (const auto& effect : effects) {
		if (effect.target.type != TargetType::CustomerAccount) 
			continue;

		deltas[effect.target.targetId] += effect.delta;
	}

	//validation
	for (const auto& [accountId,delta] : deltas) {
		Account* acc = accountRepo.FindById(accountId);

		if (!acc) {
			throw std::logic_error("Account not found: id=" + std::to_string(accountId));
		}

		if (acc->GetBalance() + delta < Money(0)) {
			throw std::logic_error("Invalid balance after apply: id=" + std::to_string(accountId));
		}
	}


	//commit
	for(const auto& [accountId,delta] : deltas){
		if (delta == Money(0))
			continue;

		Account* acc = accountRepo.FindById(accountId);
		acc->ApplyDelta(delta);
	}
}


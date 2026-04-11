	#pragma once
	#include "../Accounts/Account.h"


	#include <chrono>
	#include <iostream>

	class DepositAccount : public Account {
		static const int VaultId = 100;
		static const int BankRevenueId = 101;

	private:
		std::chrono::system_clock::time_point createdAccountDate;
		std::chrono::system_clock::time_point matuiryDateEnd;
		double penaltyPercent;
		int months;

	public:
		 DepositAccount(int months, double interestRate)
			 : Account(AccountType::Deposit),
			 createdAccountDate(std::chrono::system_clock::now()),
			 matuiryDateEnd(createdAccountDate + std::chrono::hours(months * 30 * 24)),
			penaltyPercent(interestRate),
			months(months){
		 }
		 DepositAccount(int id, AccountStatus status, Money balance, int months, double interestRate)
			 : Account(AccountType::Deposit, id, status, balance),
			 createdAccountDate(std::chrono::system_clock::now()),
			 matuiryDateEnd(createdAccountDate + std::chrono::hours(months * 30 * 24)),
			 penaltyPercent(interestRate),
			 months(months) {
		 }

		 WithdrawResult CanWithdraw(const Money& amount) const {

			 auto now = std::chrono::system_clock::now();

			 auto total = amount;

			 if (now < matuiryDateEnd) {
				 total += GetPenalty(amount);
			 }

			 if (total > GetBalance()) {
				 return WithdrawResult::InsufficientFunds;
			 }

			 return Account::BaseCanWithdraw(total);
		 }

		 std::vector<Effect> Withdraw(const Money& amount) override {


			 auto now = std::chrono::system_clock::now();

			 auto total = amount;

			 if (now < matuiryDateEnd) {
				 auto penalty = GetPenalty(amount);
				 if (penalty != Money(0)) {
					 return {
								Effect(EffectTarget{TargetType::CustomerAccount, GetId()}, -amount,  EffectReason::Withdraw),
								Effect(EffectTarget{TargetType::Vault, VaultId},                -amount,  EffectReason::Withdraw),
								Effect(EffectTarget{TargetType::CustomerAccount, GetId()}, -penalty, EffectReason::EarlyWithdrawalPenalty),
								Effect(EffectTarget{TargetType::BankRevenue, BankRevenueId},  penalty, EffectReason::EarlyWithdrawalPenalty)
					 };
				 }
			 }

			 return {
			Effect(EffectTarget{TargetType::CustomerAccount, GetId()}, -amount, EffectReason::Withdraw),
			Effect(EffectTarget{TargetType::Vault, VaultId},                -amount, EffectReason::Withdraw)
			 };

		 }

		DepositResult CanDeposit(const Money& amount) const override {
			return Account::BaseCanDeposit(amount);
		}

	public:
		Money GetPenalty(const Money& amount) const override {
			return (amount * penaltyPercent);
		}

	public:
		const double GetInterestRate() const {
			return penaltyPercent;
		}
	public:
		const int GetMonths() const {
			return months;
		}


	};


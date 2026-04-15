
#include <iostream>
#include <chrono>

#include "InterestAccrualService.h"
#include "../Accounts/SavingsAccount.h"

void InterestAccrualService::AccrueAll(){
	auto savingAccounts = accRepo.FindAllSavings();

	for (auto& account : savingAccounts) {
		auto now = std::chrono::system_clock::now();
		auto diff = now - account->GetLastAccrualDate();
		auto days = std::chrono::duration_cast<std::chrono::days>(diff).count();

		if (days >= 30) {
			auto interestAmount = account->GetInterestRate() * (account->GetBalance().GetCents() / 100);
			processor.Deposit(account->GetId(),Money(interestAmount));
			account->UpdateLastAccrualDate();
			accRepo.Update(*account);
		}
	}
}
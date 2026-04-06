#include "AccountRepository.h"
#include <iostream>
#include <iomanip>


void AccountRepository::Add(std::unique_ptr<Account> account) {
	accounts.push_back(std::move(account));
}


bool AccountRepository::Exists(int id) const {
	for (const auto& acc : accounts) {
		if (acc->GetId() == id) {
			return true;
		}
	}
	return false;
}


Account* AccountRepository::FindById(int id) {
	for (auto& acc : accounts) {
		if (acc->GetId() == id) {
			return acc.get();
		}
	}
	return nullptr;
}

void AccountRepository::PrintAll() const {
	for (auto& acc : accounts) {

		std::cout << "Id: " << acc->GetId() << " | "
				  << "Balance: " <<  acc->GetBalance().ToString() << " | "
				  << "Status: " << AccountStatusToString(acc->GetStatus()) << "\n";
	} 
}



std::string AccountRepository::AccountStatusToString(AccountStatus status) const {
	switch (status) {
	case AccountStatus::Active: return "Active";
	case AccountStatus::Frozen: return "Frozen";
	case AccountStatus::Blocked: return "Blocked";
	case AccountStatus::Closed: return "Closed";
	default: return "Unknown";
	}
}

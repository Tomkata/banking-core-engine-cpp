#include "catch_amalgamated.hpp"
#include <chrono>
#include <iostream>

#include "FakeAccountRepository.h"
#include "FakeTransactionRepository.h"
#include "FakeUnitOfWork.h"
#include "ProcessorFixture.h"


#include "../../MiniBankingCore/Domain/services/TransactionProcessor.h"
#include "../../MiniBankingCore/Domain/Models/services/InterestAccrualService.h"


TEST_CASE("InterestAccrualService: Interest accrual shoud deposit efter 30 days") {

	ProcessorFixture pf;
	auto pastDate = std::chrono::system_clock::now() - std::chrono::days(31);

	pf.accountRepo.Seed(AccountType::Saving, 1, AccountStatus::Active, Money(10000), 0, 0.02, pastDate);
	pf.interestAccrual.AccrueAll();

	auto acc = pf.accountRepo.FindById(1);

	REQUIRE(acc->GetBalance() == Money(10200));
}


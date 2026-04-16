#include "catch_amalgamated.hpp"

#include "FakeAccountRepository.h"
#include "FakeTransactionRepository.h"
#include "FakeUnitOfWork.h"
#include "ProcessorFixture.h"


#include "../../MiniBankingCore/Domain/Mapper/EntryMapper.h"
#include "../../MiniBankingCore/Domain/Models/services/TransferFeeCalculator.h"
#include "../../MiniBankingCore/Domain/Models/services/TransferOperation.h"
#include "../../MiniBankingCore/Domain/services/TransactionProcessor.h"
#include "../../MiniBankingCore/Domain/Exceptions/Account/InvalidWithdrawException.h"
#include "../../MiniBankingCore/Domain/Exceptions/Account/InvalidDepositException.h"


TEST_CASE("EntryMapper: Withdraw from CustomerAccount returns Liability Debit entry"
) {
	ProcessorFixture pf;
	pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(1000));

	auto acc = pf.accountRepo.FindById(1);

	auto effects = acc->Withdraw(Money(100));
	
	auto entries = pf.mapper.Map(effects.Value()[0]);

	REQUIRE(entries.size() == 1);
	REQUIRE(entries[0].GetLedgerType() == LedgerAccountType::Liability);
	REQUIRE(entries[0].GetType() == TransactionEntryType::Debit);
}


TEST_CASE("EntryMapper: Deposit from CustomerAccount returns Liability Credit entry") {
	ProcessorFixture pf;
	pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(1000));

	auto acc = pf.accountRepo.FindById(1);

	auto effects = acc->Deposit(Money(100));

	auto entries = pf.mapper.Map(effects.Value()[0]);

	REQUIRE(entries.size() == 1);
	REQUIRE(entries[0].GetLedgerType() == LedgerAccountType::Liability);
	REQUIRE(entries[0].GetType() == TransactionEntryType::Credit);
}

TEST_CASE("EntryMapper: Withdraw from Vault returns Asset Credit entry") {
	ProcessorFixture pf;
	pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(1000));

	auto acc = pf.accountRepo.FindById(1);

	auto effects = acc->Withdraw(Money(100));

	auto entries = pf.mapper.Map(effects.Value()[1]);

	REQUIRE(entries.size() == 1);
	REQUIRE(entries[0].GetLedgerType() == LedgerAccountType::Asset);
	REQUIRE(entries[0].GetType() == TransactionEntryType::Credit);
}

TEST_CASE("EntryMapper: Deposit to Vault returns Asset Debit entry") {
	ProcessorFixture pf;
	pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(1000));

	auto acc = pf.accountRepo.FindById(1);

	auto effects = acc->Deposit(Money(100));

	auto entries = pf.mapper.Map(effects.Value()[1]);

	REQUIRE(entries.size() == 1);
	REQUIRE(entries[0].GetLedgerType() == LedgerAccountType::Asset);
	REQUIRE(entries[0].GetType() == TransactionEntryType::Debit);
}


TEST_CASE("EntryMapper: Transfer from source account returns Liability Debit entry") {
	ProcessorFixture pf;
	pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(1000));
	pf.accountRepo.Seed(AccountType::Checking, 2, AccountStatus::Active, Money(100));

	auto acc1 = pf.accountRepo.FindById(1);
	auto acc2 = pf.accountRepo.FindById(2);

	Money fee(0);

	auto effects = pf.transferOperation.CreateEffects(*acc1,*acc2,Money(10), fee);

	auto entries = pf.mapper.Map(effects.Value()[0]);

	REQUIRE(entries.size() == 1);
	REQUIRE(entries[0].GetLedgerType() == LedgerAccountType::Liability);
	REQUIRE(entries[0].GetType() == TransactionEntryType::Debit);
}

TEST_CASE("EntryMapper: Transfer with fee effect shoud return Revenue Credit Entry") {
	ProcessorFixture pf;
	pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(1000));
	pf.accountRepo.Seed(AccountType::Checking, 2, AccountStatus::Active, Money(100));

	auto acc1 = pf.accountRepo.FindById(1);
	auto acc2 = pf.accountRepo.FindById(2);

	Money fee(10);

	auto effects = pf.transferOperation.CreateEffects(*acc1, *acc2, Money(10), fee);

	auto entries = pf.mapper.Map(effects.Value()[2]);

	REQUIRE(entries.size() == 1);
	REQUIRE(entries[0].GetLedgerType() == LedgerAccountType::Revenue);
	REQUIRE(entries[0].GetType() == TransactionEntryType::Credit);
}

TEST_CASE("EntryMapper: EffectReasonToString returns correct string") {
	REQUIRE(EntryMapper::EffectReasonToString(EffectReason::Deposit) == "Deposit");
	REQUIRE(EntryMapper::EffectReasonToString(EffectReason::Withdraw) == "Withdraw");
	REQUIRE(EntryMapper::EffectReasonToString(EffectReason::Transfer) == "Transfer");
}
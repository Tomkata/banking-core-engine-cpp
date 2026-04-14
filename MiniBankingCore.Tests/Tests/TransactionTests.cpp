#define CATCH_CONFIG_MAIN 
#include "catch_amalgamated.hpp"

#include "FakeAccountRepository.h"
#include "FakeTransactionRepository.h"
#include "FakeUnitOfWork.h"

#include "../../MiniBankingCore/Domain/Mapper/EntryMapper.h"
#include "../../MiniBankingCore/Domain/Models/services/TransferFeeCalculator.h"
#include "../../MiniBankingCore/Domain/Models/services/TransferOperation.h"
#include "../../MiniBankingCore/Domain/services/TransactionProcessor.h"
#include "../../MiniBankingCore/Domain/Exceptions/Account/InvalidWithdrawException.h"
#include "../../MiniBankingCore/Domain/Exceptions/Account/InvalidDepositException.h"

TEST_CASE("TransactionProccessor: Deposit increase account balance") {
    FakeAccountRepository accountRepo;
    FakeTransactionRepository transactionRepo;
    FakeUnitOfWork uow;
    EntryMapper mapper;
    TransferFeeCalculator calc;
    TransferOperation transferOperation;

    accountRepo.Seed(AccountType::Checking,1,AccountStatus::Active,Money(0));

    TransactionProcessor trProccessor(accountRepo,transactionRepo,mapper, uow,calc,transferOperation);

    trProccessor.Deposit(1,Money(100));

    auto acc = accountRepo.FindById(1);

    REQUIRE(acc->GetBalance() == Money(100));
}

TEST_CASE("TransactionProccessor: deposit with closed account should thrown exception") {
    FakeAccountRepository accountRepo;
    FakeTransactionRepository transactionRepo;
    FakeUnitOfWork uow;
    EntryMapper mapper;
    TransferFeeCalculator calc;
    TransferOperation transferOperation;

    accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Closed, Money(0));
    TransactionProcessor processor(accountRepo, transactionRepo, mapper, uow, calc, transferOperation);


    REQUIRE_THROWS_AS(processor.Deposit(1, Money(100)), InvalidDepositException);
}


TEST_CASE("TransactionProccessor: Withdraw decrease account balance") {
    FakeAccountRepository accountRepo;
    FakeTransactionRepository transactionRepo;
    FakeUnitOfWork uow;
    EntryMapper mapper;
    TransferFeeCalculator calc;
    TransferOperation transferOperation;

    accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(200));

    TransactionProcessor trProccessor(accountRepo, transactionRepo, mapper, uow, calc, transferOperation);

    trProccessor.Withdraw(1, Money(100));

    auto acc = accountRepo.FindById(1);

    REQUIRE(acc->GetBalance() == Money(100));
}

TEST_CASE("TransactionProccessor: Invalid withdraw shoud thow exception") {
    FakeAccountRepository accountRepo;
    FakeTransactionRepository transactionRepo;
    FakeUnitOfWork uow;
    EntryMapper mapper;
    TransferFeeCalculator calc;
    TransferOperation transferOperation;

    accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(200));

    TransactionProcessor trProccessor(accountRepo, transactionRepo, mapper, uow, calc, transferOperation);

    REQUIRE_THROWS_AS(trProccessor.Withdraw(1, Money(-100)), InvalidWithdrawException);

}

#define CATCH_CONFIG_MAIN 
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





TEST_CASE("TransactionProccessor: Deposit increase account balance") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Checking,1,AccountStatus::Active,Money(0));


    pf.processor.Deposit(1,Money(100));

    auto acc = pf.accountRepo.FindById(1);

    REQUIRE(acc->GetBalance() == Money(100));
}

TEST_CASE("TransactionProccessor: deposit with closed account should thrown exception") {
    ProcessorFixture pf;

    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Closed, Money(200));

    REQUIRE_THROWS_AS(pf.processor.Deposit(1, Money(100)), InvalidDepositException);
}

TEST_CASE("TransactionProccessor: deposit with blocked account should thrown exception") {
    ProcessorFixture pf;

    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Blocked, Money(200));

    REQUIRE_THROWS_AS(pf.processor.Deposit(1, Money(100)), InvalidDepositException);
}

TEST_CASE("TransactionProccessor: Withdraw decrease account balance") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(200));

   pf.processor.Withdraw(1, Money(100));

    auto acc = pf.accountRepo.FindById(1);

    REQUIRE(acc->GetBalance() == Money(100));
}

TEST_CASE("TransactionProccessor: Invalid withdraw shoud throw exception") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(0));


    REQUIRE_THROWS_AS(pf.processor.Withdraw(1, Money(-100)), InvalidWithdrawException);
}

TEST_CASE("TransactionProccessor: Withdraw from save account shoud throw exception") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Saving, 1, AccountStatus::Active, Money(0));


    REQUIRE_THROWS_AS(pf.processor.Withdraw(1, Money(100)), InvalidWithdrawException);
}

TEST_CASE("TransactionProccessor: Transfer between two accounts shoud be successful") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(200));
    pf.accountRepo.Seed(AccountType::Checking, 2, AccountStatus::Active, Money(0));

    pf.processor.Transfer(1, 2, Money(100));
    
    auto acc1 = pf.accountRepo.FindById(1);
    auto acc2 = pf.accountRepo.FindById(2);


    REQUIRE(acc1->GetBalance() == Money(100));
    REQUIRE(acc2->GetBalance() == Money(100));
}

TEST_CASE("TransactionProccessor: Transfer to invalid account shoud throw logic error") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(200));


    REQUIRE_THROWS_AS(pf.processor.Transfer(1, -1, Money(100)),std::logic_error);
    
}


TEST_CASE("TransactionProccessor: Transfer from frozen account shoud throw exception") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Frozen, Money(200));
    pf.accountRepo.Seed(AccountType::Checking, 2, AccountStatus::Active, Money(200));


    REQUIRE_THROWS_AS(pf.processor.Transfer(1, 2, Money(100)), InvalidWithdrawException);

}

TEST_CASE("TransactionProccessor: Transfer with insufficient balance shoud throw exception") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Frozen, Money(100));
    pf.accountRepo.Seed(AccountType::Checking, 2, AccountStatus::Active, Money(0));


    REQUIRE_THROWS_AS(pf.processor.Transfer(1, 2, Money(1000)), InvalidWithdrawException);

}

TEST_CASE("Transaction is balanced") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(100));
    pf.accountRepo.Seed(AccountType::Checking, 2, AccountStatus::Active, Money(0));

     pf.processor.Transfer(1,2,Money(100));

     auto& transactions = pf.transactionRepo.GetAll();

     REQUIRE(transactions.size() == 1);
     REQUIRE(transactions[0].isBalanced());
}

//TEST_CASE("Transaction is atomic - failure rolls back all changes") {
//    ProcessorFixture pf;
//
//    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(200));
//    pf.accountRepo.Seed(AccountType::Checking, 2, AccountStatus::Active, Money(0));
//
//    pf.accountRepo.SetFailOnUpdate(2);
//
//    REQUIRE_THROWS(pf.processor.Transfer(1, 2, Money(100)));
//
//    auto acc1Balance = pf.accountRepo.FindById(1)->GetBalance();
//    auto acc2Balance = pf.accountRepo.FindById(2)->GetBalance();
//
//    REQUIRE(acc1Balance == Money(200));
//    REQUIRE(acc2Balance == Money(0));
//}



TEST_CASE("Deposit account witdraw shoud have penatly before maturity date") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Deposit, 1, AccountStatus::Active, Money(1000),6,0.02);

    pf.processor.Withdraw(1,Money(100));
    auto acc = pf.accountRepo.FindById(1);


    REQUIRE(acc->GetBalance() == Money(898));
}

TEST_CASE("Deposit account witdraw shoud not have panatly after maturity date") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Deposit, 1, AccountStatus::Active, Money(1000), 0, 0.02);

    pf.processor.Withdraw(1, Money(100));
    auto acc = pf.accountRepo.FindById(1);


    REQUIRE(acc->GetBalance() == Money(900));
}

TEST_CASE("Deposit account early withdraw with insufficient balance for penalty should throw") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Deposit, 1, AccountStatus::Active, Money(1000), 6, 0.02);


    REQUIRE_THROWS_AS(pf.processor.Withdraw(1, Money(1000)), InvalidWithdrawException);
}

TEST_CASE("Deposit account withdraw with exact avalaible balance shoud be successful") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Deposit, 1, AccountStatus::Active, Money(1000), 6, 0.02);

    pf.processor.Withdraw(1, Money(980));
    auto acc = pf.accountRepo.FindById(1);


    REQUIRE(acc->GetBalance() == Money(0.4));
}


TEST_CASE("Withdraw generates correct effects") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(1000), 6, 0.02);

    auto acc = pf.accountRepo.FindById(1);

    auto effects = acc->Withdraw(Money(100));

    REQUIRE(effects.size() == 2);
    REQUIRE(effects[0].delta == Money(-100));
}

TEST_CASE("Deposit generates correct effects") {
    ProcessorFixture pf;
    pf.accountRepo.Seed(AccountType::Checking, 1, AccountStatus::Active, Money(1000), 6, 0.02);

    auto acc = pf.accountRepo.FindById(1);

    auto effects = acc->Deposit(Money(100));

    REQUIRE(effects.size() == 2);
    REQUIRE(effects[0].delta == Money(100));
}
#define CATCH_CONFIG_MAN
#include "catch_amalgamated.hpp"
#include "../../MiniBankingCore/Domain/Models/Accounts/CheckingAccount.h"
#include "../../MiniBankingCore/Domain/Exceptions/Account/InvalidDepositException.h"

TEST_CASE("CheckingAccount::Deposit returns 2 effects on valid deposit") {
	
	CheckingAccount acc;

	auto effects = acc.Deposit(Money(100));

	REQUIRE(effects.size() == 2);
}

TEST_CASE("CheckingAccount::Deposit throws on closed account") {
    CheckingAccount acc;
    acc.Close();

    REQUIRE_THROWS_AS(acc.Deposit(Money(100)), InvalidDepositException);
}
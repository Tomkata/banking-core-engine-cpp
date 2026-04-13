#pragma once
#include <iostream>
#include <vector>
#include "../Effects/Effect.h"
#include "../../ValueObjects/Money.h"

class TransferFeeCalculator {
	static const int VaultId = 100;
	static const int BankRevenueId = 101;

	const long long MinFee = 20;
	const long long MaxFee = 500;

public:
	Money CalculateFee(double interest, const Money& amount);
};

#include <algorithm>
#include "TransferFeeCalculator.h"

// Calculates transfer fee in cents using clamp(amount * rate, minFee, maxFee)
// MinFee = 0.20 lv, MaxFee = 5.00 lv
Money TransferFeeCalculator::CalculateFee(double interest, const Money& amount) {
	if (interest == 0.0) return Money(0);
	auto raw = amount * interest;
	auto fee = std::clamp(raw.GetCents(), MinFee, MaxFee);

	return Money(fee);
}
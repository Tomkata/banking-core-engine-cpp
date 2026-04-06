#pragma once
enum class EffectReason {
    Withdraw,
    Deposit,
    Transfer,
    EarlyWithdrawalPenalty,
    TransferFee,
    MonthlyMaintenanceFee,
    Tax
};
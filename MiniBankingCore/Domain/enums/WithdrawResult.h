#pragma once
enum class WithdrawResult {
    Ok,
    InsufficientFunds,
    Frozen,
    NotSupported,
    Closed,
    Blocked,
    NotMatured,
    EarlyWithdrawal,
    InvalidAmount
};

#pragma once
#include "../enums/DepositResult.h"
#include "../enums/WithdrawResult.h"
#include <stdexcept>


enum class TransferResult {
    Ok,
    SameAccount,
    InsufficientFunds,
    SourceFrozen,
    SourceBlocked,
    SourceClosed,
    DestinationBlocked,
    DestinationClosed,
    InvalidAmount
};

static TransferResult FromWithdrawResult(WithdrawResult result) {
    switch (result) {
    case WithdrawResult::InsufficientFunds: return TransferResult::InsufficientFunds;
    case WithdrawResult::Frozen:            return TransferResult::SourceFrozen;
    case WithdrawResult::Blocked:           return TransferResult::SourceBlocked;
    case WithdrawResult::Closed:            return TransferResult::SourceClosed;
    case WithdrawResult::InvalidAmount:     return TransferResult::InvalidAmount;
    default: throw std::logic_error("Unknown WithdrawResult");
    }
}

static TransferResult FromDepositResult(DepositResult result) {
    switch (result) {
    case DepositResult::Blocked:  return TransferResult::DestinationBlocked;
    case DepositResult::Closed:   return TransferResult::DestinationClosed;
    default: throw std::logic_error("Unknown DepositResult");
    }
}
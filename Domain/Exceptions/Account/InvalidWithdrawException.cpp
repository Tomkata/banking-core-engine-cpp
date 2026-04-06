#include "InvalidWithdrawException.h"

static std::string WithdrawResultToString(WithdrawResult result) {
    switch (result) {
    case WithdrawResult::InsufficientFunds: return "Insufficient funds";
    case WithdrawResult::Frozen:            return "Account is frozen";
    case WithdrawResult::Closed:            return "Account is closed";
    case WithdrawResult::Blocked:           return "Account is blocked";
    case WithdrawResult::NotSupported:      return "Account does not support withdrawals";
    default:                                return "Withdraw not allowed";
    }
}

InvalidWithdrawException::InvalidWithdrawException(WithdrawResult result)
    : message(WithdrawResultToString(result)) {
}

const char* InvalidWithdrawException::what() const noexcept {
    return message.c_str();
}
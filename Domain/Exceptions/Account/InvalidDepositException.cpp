#include "InvalidDepositException.h"


    static std::string DepositResultToString(DepositResult result) {
        switch (result) {
        case DepositResult::InsufficientFunds: return "Insufficient funds";
        case DepositResult::Closed:            return "Account is closed";
        case DepositResult::Blocked:           return "Account is blocked";
        case DepositResult::NotSupported:      return "Account does not support deposit";
        default:                                return "Deposit not allowed";
        }
    }


InvalidDepositException::InvalidDepositException(DepositResult result)
    : message(DepositResultToString(result)) {
}
    
const char* InvalidDepositException::what() const noexcept {
    return message.c_str();
}
    
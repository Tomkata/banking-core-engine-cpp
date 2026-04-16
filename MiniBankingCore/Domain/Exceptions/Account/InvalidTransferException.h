#pragma once
#include <stdexcept>
#include <string>
#include "../../enums/TransferResult.h"

class InvalidTransferException : public std::exception {
private:
    std::string message;
public:
    InvalidTransferException(TransferResult result)
        : message(TransferResultToString(result)) {
    }

    const char* what() const noexcept override {
        return message.c_str();
    }

private:
    static std::string TransferResultToString(TransferResult result) {
        switch (result) {
        case TransferResult::SameAccount:        return "Cannot transfer to the same account";
        case TransferResult::InsufficientFunds:  return "Insufficient funds";
        case TransferResult::SourceFrozen:       return "Source account is frozen";
        case TransferResult::SourceBlocked:      return "Source account is blocked";
        case TransferResult::SourceClosed:       return "Source account is closed";
        case TransferResult::DestinationBlocked: return "Destination account is blocked";
        case TransferResult::DestinationClosed:  return "Destination account is closed";
        case TransferResult::InvalidAmount:      return "Invalid amount";
        default:                                 return "Transfer not allowed";
        }
    }
};
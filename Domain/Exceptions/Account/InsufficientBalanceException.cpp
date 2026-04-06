#include "InsufficientBalanceException.h"

InsufficientBalanceException::InsufficientBalanceException(const std::string& msg)
    : message(msg) { 
}

const char* InsufficientBalanceException::what() const noexcept {
    return message.c_str();
}
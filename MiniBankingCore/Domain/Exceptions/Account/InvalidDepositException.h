#pragma once
#include <exception>
#include <string>
#include "../../enums/DepositResult.h"

class InvalidDepositException : public std::exception {



private:
    std::string message;
public:
    explicit InvalidDepositException(DepositResult result);
    const char* what() const noexcept override;
};
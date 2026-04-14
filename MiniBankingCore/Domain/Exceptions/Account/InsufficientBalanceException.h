#pragma once
#include <exception>
#include <string>

class InsufficientBalanceException : public std::exception {
private:
    std::string message;

public:
    explicit InsufficientBalanceException(const std::string& msg);
    const char* what() const noexcept override;
};
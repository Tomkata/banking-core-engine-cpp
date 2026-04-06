#pragma once
#include "../../enums/WithdrawResult.h"

#include <string>

#include "exception"

class InvalidWithdrawException : public std::exception {

private:
	std::string message;
public:
	explicit InvalidWithdrawException(WithdrawResult result);
	const char* what() const noexcept override;
};
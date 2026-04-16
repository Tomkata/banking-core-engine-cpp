#pragma once
#include "AccountStatement.h"

class IAccountQueryService {
public:
	IAccountQueryService() = default;
	virtual AccountStatement GetStatement(int accountId) = 0;
};
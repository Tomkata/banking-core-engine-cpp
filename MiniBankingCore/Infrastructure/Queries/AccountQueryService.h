#pragma once
#include "../../Domain/Queries/IAccountQueryService.h"
class AccountQueryService : IAccountQueryService{
public:
	virtual AccountStatement GetStatement(int accountId) override;
};
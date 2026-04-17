#pragma once

#include "../../Domain/Queries/IAccountQueryService.h"
#include "../Database/Database.h"


class AccountQueryService : IAccountQueryService{
private:
	Database& db;
	
public :
	AccountQueryService(Database& db) :db(db) { }

public:
	virtual AccountStatement GetStatement(int accountId) override;
};
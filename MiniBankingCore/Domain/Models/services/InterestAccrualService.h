#pragma once

#include "../../Interfaces/IAccountRepository.h"
#include "../../services/TransactionProcessor.h"

class InterestAccrualService {
private:
	IAccountRepository& accRepo;
	TransactionProcessor& processor;

public:
	InterestAccrualService(IAccountRepository& accRepo,
		TransactionProcessor& processor):
		accRepo(accRepo),
		processor(processor){}

public:
	void AccrueAll();
};
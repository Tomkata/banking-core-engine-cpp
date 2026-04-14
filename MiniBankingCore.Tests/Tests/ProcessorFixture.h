#pragma once

#include "FakeAccountRepository.h"
#include "FakeTransactionRepository.h"
#include "FakeUnitOfWork.h"
#include "../../MiniBankingCore/Domain/Mapper/EntryMapper.h"
#include "../../MiniBankingCore/Domain/Models/services/TransferFeeCalculator.h"
#include "../../MiniBankingCore/Domain/Models/services/TransferOperation.h"
#include "../../MiniBankingCore/Domain/services/TransactionProcessor.h"

struct ProcessorFixture {
    FakeAccountRepository accountRepo;
    FakeTransactionRepository transactionRepo;
    FakeUnitOfWork uow;
    EntryMapper mapper;
    TransferFeeCalculator calc;
    TransferOperation transferOperation;
    TransactionProcessor processor;

    ProcessorFixture()
        : processor(accountRepo, transactionRepo, mapper, uow, calc, transferOperation) {
    }
};
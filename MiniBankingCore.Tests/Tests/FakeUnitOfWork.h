#pragma once
#include "../../MiniBankingCore/Domain/Interfaces/IUnitOfWork.h"

class FakeUnitOfWork : public IUnitOfWork {
public:
    int commitCount = 0;
    int rollbackCount = 0;

public:
    void BeginTransaction() override {}
    void Commit() override { commitCount++; }
    void Rollback() override { rollbackCount++; }
};
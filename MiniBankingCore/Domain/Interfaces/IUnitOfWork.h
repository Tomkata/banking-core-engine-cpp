#pragma once

class IUnitOfWork {

public:
	virtual ~IUnitOfWork() = default;
	virtual void BeginTransaction() = 0;
	virtual void Commit() = 0;
	virtual void Rollback() = 0;
};
#pragma once

#include "sqlite3.h"

struct SqliteStatementGuard {
    sqlite3_stmt* stmt;
    ~SqliteStatementGuard() { sqlite3_finalize(stmt); }
};

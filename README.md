# MiniBankingCore

A transactional banking core written in C++. Models deposit, withdrawal, and transfer operations using double-entry accounting, a domain-driven structure, and SQLite-backed persistence with full atomicity.

This is not a CRUD app. It is an exercise in applied software design — separating domain logic, accounting rules, and orchestration into distinct, testable layers.

---

## Why This Project Exists

Most banking tutorials model `account.balance -= amount` and call it a day. This project treats banking operations seriously:

- Every operation produces **balanced debit/credit entries** (double-entry accounting)
- Accounts **do not mutate state** — they return `Effect` objects describing what should change
- Database writes are wrapped in **explicit SQL transactions** (BEGIN / COMMIT / ROLLBACK)
- Business logic, accounting rules, and workflow coordination are in **separate layers**

---

## Key Features

- **Three account types:** `CheckingAccount`, `SavingsAccount`, `DepositAccount` (with maturity dates and early withdrawal penalties)
- **Effects-based domain model:** `Account::Withdraw()` and `Account::Deposit()` return `std::vector<Effect>` instead of mutating state
- **Double-entry accounting:** Every operation maps to balanced debit/credit `TransactionEntry` records via `EntryMapper`
- **Transfer fees:** Calculated as a percentage of amount, clamped between a min and max (20–500 cents)
- **Early withdrawal penalty:** `DepositAccount` charges a penalty fee before maturity, routing it to `BankRevenue`
- **Repository pattern:** `IAccountRepository` / `ITransactionRepository` interfaces with SQLite implementations
- **Unit of Work:** `IUnitOfWork` wraps `BEGIN` / `COMMIT` / `ROLLBACK` for atomic multi-step writes
- **`Money` value object:** Stores amounts in `long long` cents — no floating-point arithmetic anywhere in the domain
- **Manual SQL with RAII guards:** No ORM. Queries use `sqlite3_bind_*` with a `SqliteStatementGuard` wrapper

---

## Architecture Overview

```
MiniBankingCore/
├── Domain/
│   ├── ValueObjects/
│   │   └── Money                      # Integer cents, full operator set
│   ├── Models/
│   │   ├── Accounts/
│   │   │   ├── Account                # Abstract base: balance, status, ApplyDelta
│   │   │   ├── CheckingAccount        # No restrictions
│   │   │   ├── SavingsAccount         # Withdrawals blocked
│   │   │   └── DepositAccount         # Maturity date, penalty on early withdrawal
│   │   ├── Effects/
│   │   │   ├── Effect                 # {EffectTarget, Money delta, EffectReason}
│   │   │   └── EffectTarget           # {TargetType, targetId}
│   │   ├── Transaction                # Aggregate: type, status, entries, failure reason
│   │   ├── TransactionEntry           # Single debit or credit line
│   │   └── services/
│   │       ├── TransferFeeCalculator  # Fee = clamp(amount * rate, min, max)
│   │       └── TransferOperation      # Builds transfer Effect set
│   ├── Mapper/
│   │   └── EntryMapper                # Effect → TransactionEntry (debit/credit logic)
│   ├── Interfaces/
│   │   ├── IAccountRepository
│   │   ├── ITransactionRepository
│   │   └── IUnitOfWork
│   ├── services/
│   │   └── TransactionProcessor       # Orchestrates all operations
│   └── Exceptions/                    # InsufficientBalance, InvalidDeposit, etc.
├── Infrastructure/
│   ├── Database/
│   │   ├── Database                   # sqlite3* wrapper + transaction methods
│   │   └── SqliteStatementGuard       # RAII finalizer for sqlite3_stmt
│   ├── Repositories/
│   │   ├── SqliteAccountRepository    # Polymorphic persistence with LEFT JOIN
│   │   └── SqliteTransactionRepository
│   ├── Factories/
│   │   └── AccountFactory             # Reconstructs typed Account from DB row
│   └── SqliteUnitOfWork               # Delegates BEGIN/COMMIT/ROLLBACK to Database
└── MiniBankingCore.cpp                # Wiring + schema creation + demo
```

**Layering rule:** The domain layer has zero knowledge of SQLite. Repositories are injected via interfaces. `TransactionProcessor` depends only on abstractions.

---

## Transaction Flow

Every operation (`Deposit`, `Withdraw`, `Transfer`) follows the same pipeline:

```
1. Create Transaction (status: Pending)
2. uow.BeginTransaction()            ← SQLite BEGIN
3. Load account(s) from repository
4. Validate (CanDeposit / CanWithdraw)
5. Call account.Deposit() / Withdraw() → std::vector<Effect>
6. Map each Effect → TransactionEntry via EntryMapper
7. Attach entries to Transaction
8. transaction.MarkSuccess()
9. Apply deltas to in-memory accounts (ApplyDelta)
10. Persist updated accounts to repository
11. Persist Transaction + entries to repository
12. uow.Commit()                      ← SQLite COMMIT
```

On any exception:
```
→ transaction.MarkFailed(reason)
→ uow.Rollback()                      ← SQLite ROLLBACK
→ rethrow
```

The `ExecuteTransaction` method in `TransactionProcessor` implements this as a template that accepts a lambda for the domain-specific action (step 4–7).

---

## How Effects Map to Accounting Entries

The `EntryMapper` translates domain Effects to accounting language:

| TargetType        | LedgerAccountType |
|-------------------|-------------------|
| `CustomerAccount` | Liability         |
| `Vault`           | Asset             |
| `BankRevenue`     | Revenue           |

Entry type (Debit/Credit) is then determined by the ledger type and the sign of the delta:

| Ledger Type | Positive delta | Negative delta |
|-------------|----------------|----------------|
| Asset       | Debit          | Credit         |
| Liability   | Credit         | Debit          |
| Revenue     | Credit         | Debit          |

This follows standard double-entry conventions. Every operation produces a balanced set of entries.

---

## Example: Early Withdrawal from a Deposit Account

A customer withdraws 1000.00 before the deposit account's maturity date.

`DepositAccount::Withdraw(1000_00)` returns four Effects:

```
Effect 1: CustomerAccount(id=3),  delta = -1000.00,  reason = Withdraw
Effect 2: Vault(id=100),          delta = -1000.00,  reason = Withdraw
Effect 3: CustomerAccount(id=3),  delta =   -50.00,  reason = EarlyWithdrawalPenalty
Effect 4: BankRevenue(id=101),    delta =   +50.00,  reason = EarlyWithdrawalPenalty
```

`EntryMapper` converts these to eight `TransactionEntry` records (two per Effect), maintaining the debit = credit invariant across all entries.

---

## Example: Transfer with Fee

```cpp
processor.Deposit(1, Money(50000));      // Deposit 500.00
processor.Transfer(1, 2, Money(10000)); // Transfer 100.00 from account 1 to 2
```

`TransferOperation::CreateEffects()` produces:

```
Effect 1: CustomerAccount(id=1),  delta = -100.00,  reason = Transfer
Effect 2: CustomerAccount(id=1),  delta =   -0.20,  reason = TransferFee   (min fee)
Effect 3: BankRevenue(id=101),    delta =   +0.20,  reason = TransferFee
Effect 4: CustomerAccount(id=2),  delta = +100.00,  reason = Transfer
```

---

## Database Schema

```sql
CREATE TABLE accounts (
    id            INTEGER PRIMARY KEY AUTOINCREMENT,
    type          INTEGER NOT NULL,
    status        INTEGER NOT NULL,
    balance_cents INTEGER NOT NULL
);

CREATE TABLE deposit_accounts (
    account_id    INTEGER PRIMARY KEY,
    months        INTEGER NOT NULL,
    interest_rate REAL    NOT NULL,
    FOREIGN KEY(account_id) REFERENCES accounts(id)
);

CREATE TABLE transactions (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    type            INTEGER NOT NULL,
    status          INTEGER NOT NULL,
    amount_cents    INTEGER NOT NULL,
    from_account_id INTEGER,
    to_account_id   INTEGER,
    failure_reason  TEXT
);

CREATE TABLE transaction_entries (
    id             INTEGER PRIMARY KEY,
    transaction_id INTEGER NOT NULL,
    entry_type     INTEGER NOT NULL,
    ledger_type    INTEGER NOT NULL,
    amount_cents   INTEGER NOT NULL,
    description    TEXT,
    FOREIGN KEY(transaction_id) REFERENCES transactions(id)
);
```

Polymorphic persistence: `CheckingAccount` and `SavingsAccount` live only in `accounts`. `DepositAccount` gets an additional row in `deposit_accounts`. On load, `SqliteAccountRepository` uses a `LEFT JOIN` and delegates reconstruction to `AccountFactory`.

---

## Technologies

| Component        | Technology                        |
|------------------|-----------------------------------|
| Language         | C++17                             |
| Persistence      | SQLite 3 (via C API, no ORM)      |
| Build system     | Visual Studio / MSBuild           |
| Design patterns  | Repository, Unit of Work, Factory |

---

## Key Design Decisions

**Effects instead of direct mutation**

`Account::Withdraw()` does not modify `balance`. It returns a list of `Effect` structs. The processor applies them after validation and entry creation. This means the account's business logic (what changes, for what reason) is decoupled from when and how that change is applied or persisted. It also makes the domain logic pure and easy to reason about without touching a database.

**`Money` as integer cents**

`double balance` accumulates rounding errors over repeated operations. `Money` stores `long long cents` internally. `Money::FromDouble()` converts at the boundary using `std::round`. Every comparison, addition, and subtraction stays in integer arithmetic.

**`EntryMapper` owns accounting rules, not `Account`**

Accounts know about their business rules (maturity dates, penalty rates). They do not know about debit/credit conventions. `EntryMapper` translates `EffectReason` + `TargetType` + delta sign into the correct `TransactionEntryType`. This keeps accounting logic in one place.

**Interface-based repositories**

`TransactionProcessor` depends on `IAccountRepository` and `ITransactionRepository`, not their SQLite implementations. This makes the domain layer testable without a database and allows swapping persistence backends.

**`IUnitOfWork` wrapping SQLite transactions**

Rather than calling `BEGIN`/`COMMIT` directly in the processor, `SqliteUnitOfWork` encapsulates them. The processor calls `uow.BeginTransaction()` and `uow.Rollback()` — it does not know or care that these map to SQL statements.

**Hardcoded vault and revenue IDs**

`Vault` (ID 100) and `BankRevenue` (ID 101) are system accounts defined in `BankConstants.h`. They are not customer accounts and are not stored in the `accounts` table. This is a deliberate simplification — a production system would model them as ledger accounts.

---

## What I Learned

- How to separate domain validation from persistence side effects using an Effects pattern
- How double-entry accounting maps to code: every operation produces balanced debit/credit pairs
- How to write manual, parameterized SQL in C++ without exposing raw `sqlite3*` pointers throughout the codebase
- How the Unit of Work pattern coordinates multiple repository writes under a single database transaction
- How to model polymorphic domain objects across multiple tables without an ORM
- The difference between an aggregate root that validates and produces intent versus one that directly executes its own side effects

---

## Known Limitations / Future Work

- **No unit tests.** The architecture supports it (interface-based repositories, pure domain logic), but no test suite exists yet.
- **Hardcoded system account IDs.** Vault and BankRevenue IDs are compile-time constants, not database entities.
- **`isBalanced()` uses exact double comparison.** Entry balance checking should use a tolerance or stay entirely in integer arithmetic.
- **Transfer is modeled as withdraw + deposit.** There is no first-class `TransferTransaction` type — the two legs share a single `Transaction` record but the modeling is slightly asymmetric.
- **No query layer.** There is no way to list transactions or get an account statement. Repositories only support `Add`, `Update`, and `FindById`.
- **No authentication or multi-user context.** This is a core processing engine, not an API.

---

## How to Run

**Requirements:** Visual Studio 2022 (or any C++17-compatible compiler), SQLite3 (amalgamation included).

```bash
git clone https://github.com/<your-username>/MiniBankingCore.git
cd MiniBankingCore
```

Open `MiniBankingCore.sln` in Visual Studio, build in Debug or Release, and run. The program creates `bank.db` in the working directory on first run and executes the demo operations defined in `MiniBankingCore.cpp`.

To reset state, delete `bank.db` and re-run.

---

## Project Structure at a Glance

```
Domain/         Pure business logic — no SQLite, no I/O
Infrastructure/ SQLite implementations of domain interfaces
MiniBankingCore.cpp  Wiring, schema setup, demo
```

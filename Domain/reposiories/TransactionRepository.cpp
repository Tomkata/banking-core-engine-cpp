#include "TransactionRepository.h"
#include <sstream>
#include <iomanip>

void TransactionRepository::Add(Transaction&& transaction) {
    transactions.push_back(std::move(transaction));
}

void TransactionRepository::PrintAll() const {
    for (const auto& tr : transactions) {
        std::cout << "Id: " << tr.GetId() << " | "
            << "Type: " << TransactionTypeToString(tr.GetType()) << " | "
            << "Amount: " << tr.GetAmount() << " | "
            << "Status: " << TransactionStatusToString(tr.GetStatus()) << " | "
            << "Fail reason: " << (tr.GetFailureReason().empty() ? "none" : tr.GetFailureReason()) << "\n";

        for (const auto& entry : tr.GetEntries()) {
            std::cout << "  -> "
                << (entry.GetType() == TransactionEntryType::Debit ? "DEBIT " : "CREDIT ")
                << entry.GetAmount().ToString() << " | "
                << entry.GetDescription() << "\n";
        }
    }
}


 std::string TransactionRepository::TransactionTypeToString(TransactionType type) const {
    switch (type) {
    case TransactionType::Deposit: return "Deposit";
    case TransactionType::Transfer: return "Transfer";
    case TransactionType::Withdraw: return "Withdraw";
    default:return "Unknown";
    }
}


 std::string TransactionRepository::TransactionStatusToString(TransactionStatus status) const {
     switch (status) {
     case TransactionStatus::Failed: return "Failed";
     case TransactionStatus::Pending : return "Pending";
     case TransactionStatus::Success: return "Success";
     default:return "Unknown";
     }
 }
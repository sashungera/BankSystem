#include "Transaction.h"
#include <iostream>

Transaction::Transaction(int id,
                         Type type,
                         double amount,
                         const std::string& fromIBAN,
                         const std::string& toIBAN)
    : id(id),
      type(type),
      amount(amount),
      fromIBAN(fromIBAN),
      toIBAN(toIBAN)
{
}

int Transaction::getId() const
{
    return id;
}

Transaction::Type Transaction::getType() const
{
    return type;
}

double Transaction::getAmount() const
{
    return amount;
}

std::string Transaction::getFromIBAN() const
{
    return fromIBAN;
}

std::string Transaction::getToIBAN() const
{
    return toIBAN;
}

std::string Transaction::typeToString() const
{
    switch (type)
    {
    case Type::Deposit: return "Deposit";
    case Type::Withdraw: return "Withdraw";
    case Type::Transfer: return "Transfer";
    default: return "Unknown";
    }
}

void Transaction::print() const
{
    std::cout << "Transaction ID: " << id
              << ", Type: " << typeToString()
              << ", Amount: " << amount
              << ", From: " << fromIBAN
              << ", To: " << toIBAN
              << std::endl;
}

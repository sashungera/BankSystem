#pragma once

#include <string>

class Transaction
{
public:
    enum class Type
    {
        Deposit,
        Withdraw,
        Transfer
    };

private:
    int id;
    Type type;
    double amount;
    std::string fromIBAN;
    std::string toIBAN;

public:
    Transaction(int id,
                Type type,
                double amount,
                const std::string& fromIBAN,
                const std::string& toIBAN);

    int getId() const;
    Type getType() const;
    double getAmount() const;

    std::string getFromIBAN() const;
    std::string getToIBAN() const;

    std::string typeToString() const;

    void print() const;
};
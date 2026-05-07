#pragma once

#include "Account.h"
#include <iostream>

class CheckingAccount : public Account
{
private:
    double overdraftLimit;

public:
    CheckingAccount(const std::string& iban, double balance, double overdraftLimit)
        : Account(iban, balance), overdraftLimit(overdraftLimit)
    {
    }

    double getOverdraftLimit() const
    {
        return overdraftLimit;
    }

    void setOverdraftLimit(double limit)
    {
        if (limit >= 0)
        {
            overdraftLimit = limit;
        }
    }

    bool withdraw(double amount) override
    {
        if (amount > 0 && (balance + overdraftLimit) >= amount)
        {
            balance -= amount;
            return true;
        }
        return false;
    }

    void printInfo() const override
    {
        std::cout << "[Checking Account] IBAN: " << iban
                  << ", Balance: " << balance
                  << ", Overdraft Limit: " << overdraftLimit << std::endl;
    }
};
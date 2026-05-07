#pragma once

#include "Account.h"
#include <iostream>

class SavingsAccount : public Account
{
private:
    double interestRate;

public:
    SavingsAccount(const std::string& iban, double balance, double interestRate)
        : Account(iban, balance), interestRate(interestRate)
    {
    }

    double getInterestRate() const
    {
        return interestRate;
    }

    void setInterestRate(double rate)
    {
        if (rate >= 0)
        {
            interestRate = rate;
        }
    }

    void applyInterest()
    {
        balance += balance * interestRate;
    }

    void printInfo() const override
    {
        std::cout << "[Savings Account] IBAN: " << iban
                  << ", Balance: " << balance
                  << ", Interest Rate: " << interestRate << std::endl;
    }
};
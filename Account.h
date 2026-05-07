#pragma once

#include <string>
#include <iostream>

class Account
{
protected:
    std::string iban;
    double balance;

public:
    Account(const std::string& iban, double balance = 0.0)
        : iban(iban), balance(balance)
    {
    }

    virtual ~Account() = default;

    virtual void deposit(double amount)
    {
        if (amount > 0)
        {
            balance += amount;
        }
    }

    virtual bool withdraw(double amount)
    {
        if (amount > 0 && amount <= balance)
        {
            balance -= amount;
            return true;
        }
        return false;
    }

    double getBalance() const
    {
        return balance;
    }

    std::string getIBAN() const
    {
        return iban;
    }

    // pure virtual -> abstract class
    virtual void printInfo() const = 0;
};
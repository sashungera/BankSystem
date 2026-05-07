#include "Bank.h"
#include <iostream>

Bank::Bank(const std::string& name)
    : name(name)
{
}

void Bank::addUser(const std::shared_ptr<User>& user)
{
    users.push_back(user);
}

std::shared_ptr<User> Bank::findUserById(int id) const
{
    for (const auto& user : users)
    {
        if (user->getId() == id)
        {
            return user;
        }
    }

    return nullptr;
}

void Bank::addAccount(const std::shared_ptr<Account>& account)
{
    accounts.push_back(account);
}

std::shared_ptr<Account> Bank::findAccountByIBAN(const std::string& iban) const
{
    for (const auto& account : accounts)
    {
        if (account->getIBAN() == iban)
        {
            return account;
        }
    }

    return nullptr;
}

bool Bank::deposit(const std::string& iban, double amount)
{
    auto account = findAccountByIBAN(iban);

    if (!account)
    {
        return false;
    }

    account->deposit(amount);
    return true;
}

bool Bank::withdraw(const std::string& iban, double amount)
{
    auto account = findAccountByIBAN(iban);

    if (!account)
    {
        return false;
    }

    return account->withdraw(amount);
}

bool Bank::transfer(const std::string& fromIBAN,
                    const std::string& toIBAN,
                    double amount)
{
    auto sender = findAccountByIBAN(fromIBAN);
    auto receiver = findAccountByIBAN(toIBAN);

    if (!sender || !receiver)
    {
        return false;
    }

    if (!sender->withdraw(amount))
    {
        return false;
    }

    receiver->deposit(amount);
    return true;
}

const std::string& Bank::getName() const
{
    return name;
}

void Bank::printUsers() const
{
    for (const auto& user : users)
    {
        user->print();
    }
}

void Bank::printAccounts() const
{
    for (const auto& account : accounts)
    {
        account->print();
    }
}
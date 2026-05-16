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
            return user;
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
            return account;
    }
    return nullptr;
}

bool Bank::deposit(const std::string& iban, double amount)
{
    auto account = findAccountByIBAN(iban);
    if (!account) return false;

    account->deposit(amount);

    // ✅ Record transaction
    transactions.emplace_back(nextTransactionId++,
                              Transaction::Type::Deposit,
                              amount, "", iban);
    return true;
}

bool Bank::withdraw(const std::string& iban, double amount)
{
    auto account = findAccountByIBAN(iban);
    if (!account) return false;

    bool success = account->withdraw(amount);

    // ✅ Record transaction only if successful
    if (success)
    {
        transactions.emplace_back(nextTransactionId++,
                                  Transaction::Type::Withdraw,
                                  amount, iban, "");
    }
    return success;
}

bool Bank::transfer(const std::string& fromIBAN,
                    const std::string& toIBAN,
                    double amount)
{
    auto sender   = findAccountByIBAN(fromIBAN);
    auto receiver = findAccountByIBAN(toIBAN);

    if (!sender || !receiver) return false;
    if (!sender->withdraw(amount)) return false;

    receiver->deposit(amount);

    // ✅ Record transaction
    transactions.emplace_back(nextTransactionId++,
                              Transaction::Type::Transfer,
                              amount, fromIBAN, toIBAN);
    return true;
}

const std::string& Bank::getName() const { return name; }

void Bank::printUsers() const
{
    for (const auto& user : users)
        user->print();
}

void Bank::printAccounts() const
{
    for (const auto& account : accounts)
        account->printInfo();   // ✅ was account->print() — fixed
}

void Bank::printTransactions() const   // ✅ added
{
    for (const auto& t : transactions)
        t.print();
}
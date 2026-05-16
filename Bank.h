#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Account.h"
#include "User.h"
#include "Transaction.h"


class Bank
{
private:
    std::string name;
    std::vector<std::shared_ptr<User>> users;
    std::vector<std::shared_ptr<Account>> accounts;
    std::vector<Transaction> transactions;   // ✅ added
    int nextTransactionId = 1;               // ✅ added

public:
    Bank(const std::string& name);

    void addUser(const std::shared_ptr<User>& user);
    std::shared_ptr<User> findUserById(int id) const;

    void addAccount(const std::shared_ptr<Account>& account);
    std::shared_ptr<Account> findAccountByIBAN(const std::string& iban) const;

    bool deposit(const std::string& iban, double amount);
    bool withdraw(const std::string& iban, double amount);
    bool transfer(const std::string& fromIBAN,
                  const std::string& toIBAN,
                  double amount);

    const std::string& getName() const;

    void printUsers() const;
    void printAccounts() const;
    void printTransactions() const;   // ✅ added
};
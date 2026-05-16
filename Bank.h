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
    std::vector<std::shared_ptr<User>>    users;
    std::vector<std::shared_ptr<Account>> accounts;
    std::vector<Transaction>              transactions;
    int nextTransactionId = 1;

    static const std::string ACCOUNTS_FILE;
    int getNextIbanNumber() const;           // auto-increments IBAN suffix

public:
    Bank(const std::string& name);

    void addUser(const std::shared_ptr<User>& user);
    std::shared_ptr<User> findUserById(int id) const;

    void addAccount(const std::shared_ptr<Account>& account);
    std::shared_ptr<Account> findAccountByIBAN(const std::string& iban) const;

    bool deposit(const std::string& iban, double amount);
    bool withdraw(const std::string& iban, double amount);
    bool transfer(const std::string& fromIBAN,
                  const std::string& toIBAN, double amount);

    bool deleteAccount(const std::string& iban);
    bool deleteUser(int userId);

    // Simplified: only needs userId, type, extra (rate or overdraft)
    std::shared_ptr<Account> createAccountForUser(int userId,
                                                   const std::string& type,
                                                   double extra);

    void printUsersWithAccounts() const;
    void printTransactionsForUser(const std::string& iban) const;

    const std::string& getName() const;
    void printUsers()    const;
    void printAccounts() const;
    void printTransactions() const;

    // Persistence
    void saveAccounts() const;
    void loadAccounts(/* users must be loaded first */);
};
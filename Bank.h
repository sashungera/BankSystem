#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Account.h"
#include "User.h"

class Bank
{
private:
    std::string name;

    std::vector<std::shared_ptr<User>> users;
    std::vector<std::shared_ptr<Account>> accounts;

public:
    Bank(const std::string& name);

    // User management
    void addUser(const std::shared_ptr<User>& user);
    std::shared_ptr<User> findUserById(int id) const;

    // Account management
    void addAccount(const std::shared_ptr<Account>& account);
    std::shared_ptr<Account> findAccountByIBAN(const std::string& iban) const;

    // Operations
    bool deposit(const std::string& iban, double amount);
    bool withdraw(const std::string& iban, double amount);
    bool transfer(const std::string& fromIBAN,
                  const std::string& toIBAN,
                  double amount);

    // Getters
    const std::string& getName() const;

    void printUsers() const;
    void printAccounts() const;
};
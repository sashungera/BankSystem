#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Account.h"

class User
{
private:
    int id;
    std::string name;
    std::vector<std::shared_ptr<Account>> accounts;

public:
    User(int id, const std::string& name)
        : id(id), name(name)
    {
    }

    int getId() const
    {
        return id;
    }

    std::string getName() const
    {
        return name;
    }

    void addAccount(const std::shared_ptr<Account>& account)
    {
        accounts.push_back(account);
    }

    void removeAccount(const std::string& iban)
    {
        for (auto it = accounts.begin(); it != accounts.end(); ++it)
        {
            if ((*it)->getIBAN() == iban)
            {
                accounts.erase(it);
                return;
            }
        }
    }

    const std::vector<std::shared_ptr<Account>>& getAccounts() const
    {
        return accounts;
    }

    void print() const
    {
        std::cout << "User ID: " << id << ", Name: " << name << std::endl;

        for (const auto& acc : accounts)
        {
            acc->printInfo();
        }
    }
};
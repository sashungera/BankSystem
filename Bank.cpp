#include "Bank.h"
#include <iostream>
#include "SavingsAccount.h"
#include "CheckingAccount.h"
#include <algorithm>

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
void Bank::printTransactionsForUser(const std::string& userIban) const
{
    bool found = false;
    for (const auto& t : transactions)
    {
        if (t.getFromIBAN() == userIban || t.getToIBAN() == userIban)
        {
            t.print();
            found = true;
        }
    }
    if (!found) std::cout << "  (no transactions for this IBAN)\n";
}
bool Bank::deleteAccount(const std::string& iban)
{
    for (auto it = accounts.begin(); it != accounts.end(); ++it)
    {
        if ((*it)->getIBAN() == iban)
        {
            // also remove from any user who owns it
            for (auto& user : users)
                user->removeAccount(iban);

            accounts.erase(it);
            return true;
        }
    }
    return false;
}

bool Bank::deleteUser(int userId)
{
    for (auto it = users.begin(); it != users.end(); ++it)
    {
        if ((*it)->getId() == userId)
        {
            // remove all accounts belonging to this user
            for (const auto& acc : (*it)->getAccounts())
            {
                const std::string& iban = acc->getIBAN();
                accounts.erase(
                    std::remove_if(accounts.begin(), accounts.end(),
                        [&](const std::shared_ptr<Account>& a){
                            return a->getIBAN() == iban;
                        }),
                    accounts.end());
            }
            users.erase(it);
            return true;
        }
    }
    return false;
}

std::shared_ptr<Account> Bank::createAccountForUser(int userId,
                                                     const std::string& type,
                                                     const std::string& iban,
                                                     double balance,
                                                     double extra)
{
    auto user = findUserById(userId);
    if (!user) return nullptr;

    std::shared_ptr<Account> acc;
    if (type == "savings")
        acc = std::make_shared<SavingsAccount>(iban, balance, extra);
    else if (type == "checking")
        acc = std::make_shared<CheckingAccount>(iban, balance, extra);
    else
        return nullptr;

    user->addAccount(acc);
    addAccount(acc);
    return acc;
}

void Bank::printUsersWithAccounts() const
{
    for (const auto& user : users)
    {
        std::cout << "  User #" << user->getId()
                  << " — " << user->getName() << "\n";
        const auto& accs = user->getAccounts();
        if (accs.empty())
            std::cout << "    (no accounts)\n";
        else
            for (const auto& acc : accs)
            {
                std::cout << "    ";
                acc->printInfo();
            }
    }
}
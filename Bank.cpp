#include "Bank.h"
#include "SavingsAccount.h"
#include "CheckingAccount.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

const std::string Bank::ACCOUNTS_FILE = "accounts.txt";

Bank::Bank(const std::string& name) : name(name) {}

// ── IBAN generation ────────────────────────────────────────────────────────
int Bank::getNextIbanNumber() const
{
    // Also check the file so the counter survives restarts
    int maxNum = (int)accounts.size();

    std::ifstream file(ACCOUNTS_FILE);
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string iban;
        std::getline(ss, iban, '|');
        // IBANs are "BG" + number
        if (iban.size() > 2)
        {
            try {
                int n = std::stoi(iban.substr(2));
                maxNum = std::max(maxNum, n);
            } catch (...) {}
        }
    }
    return maxNum + 1;
}

// ── Users ──────────────────────────────────────────────────────────────────
void Bank::addUser(const std::shared_ptr<User>& user)
{
    users.push_back(user);
}

std::shared_ptr<User> Bank::findUserById(int id) const
{
    for (const auto& u : users)
        if (u->getId() == id) return u;
    return nullptr;
}

// ── Accounts ───────────────────────────────────────────────────────────────
void Bank::addAccount(const std::shared_ptr<Account>& account)
{
    accounts.push_back(account);
}

std::shared_ptr<Account> Bank::findAccountByIBAN(const std::string& iban) const
{
    for (const auto& a : accounts)
        if (a->getIBAN() == iban) return a;
    return nullptr;
}

std::shared_ptr<Account> Bank::createAccountForUser(int userId,
                                                     const std::string& type,
                                                     double extra)
{
    auto user = findUserById(userId);
    if (!user) return nullptr;

    std::string iban = "BG" + std::to_string(getNextIbanNumber());

    std::shared_ptr<Account> acc;
    if (type == "savings")
        acc = std::make_shared<SavingsAccount>(iban, 0.0, extra);
    else if (type == "checking")
        acc = std::make_shared<CheckingAccount>(iban, 0.0, extra);
    else
        return nullptr;

    user->addAccount(acc);
    addAccount(acc);
    saveAccounts();   // persist immediately
    return acc;
}

bool Bank::deleteAccount(const std::string& iban)
{
    for (auto it = accounts.begin(); it != accounts.end(); ++it)
    {
        if ((*it)->getIBAN() == iban)
        {
            for (auto& u : users) u->removeAccount(iban);
            accounts.erase(it);
            saveAccounts();
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
            saveAccounts();
            return true;
        }
    }
    return false;
}

// ── Transactions ───────────────────────────────────────────────────────────
bool Bank::deposit(const std::string& iban, double amount)
{
    auto acc = findAccountByIBAN(iban);
    if (!acc) return false;
    acc->deposit(amount);
    transactions.emplace_back(nextTransactionId++,
                              Transaction::Type::Deposit, amount, "", iban);
    saveAccounts();
    return true;
}

bool Bank::withdraw(const std::string& iban, double amount)
{
    auto acc = findAccountByIBAN(iban);
    if (!acc) return false;
    if (!acc->withdraw(amount)) return false;
    transactions.emplace_back(nextTransactionId++,
                              Transaction::Type::Withdraw, amount, iban, "");
    saveAccounts();
    return true;
}

bool Bank::transfer(const std::string& fromIBAN,
                    const std::string& toIBAN, double amount)
{
    auto sender   = findAccountByIBAN(fromIBAN);
    auto receiver = findAccountByIBAN(toIBAN);
    if (!sender || !receiver)   return false;
    if (!sender->withdraw(amount)) return false;
    receiver->deposit(amount);
    transactions.emplace_back(nextTransactionId++,
                              Transaction::Type::Transfer,
                              amount, fromIBAN, toIBAN);
    saveAccounts();
    return true;
}

// ── Persistence ────────────────────────────────────────────────────────────
// Format per line: IBAN|type|balance|extra|userId
void Bank::saveAccounts() const
{
    std::ofstream file(ACCOUNTS_FILE);
    if (!file.is_open()) { std::cout << "  [!] Could not save accounts.\n"; return; }

    for (const auto& acc : accounts)
    {
        // Find owner
        int ownerId = -1;
        for (const auto& u : users)
            for (const auto& a : u->getAccounts())
                if (a->getIBAN() == acc->getIBAN())
                    ownerId = u->getId();

        // Determine type and extra value
        std::string type;
        double extra = 0.0;

        if (auto* sa = dynamic_cast<SavingsAccount*>(acc.get()))
        {
            type  = "savings";
            extra = sa->getInterestRate();
        }
        else if (auto* ca = dynamic_cast<CheckingAccount*>(acc.get()))
        {
            type  = "checking";
            extra = ca->getOverdraftLimit();
        }

        file << acc->getIBAN()    << "|"
             << type              << "|"
             << acc->getBalance() << "|"
             << extra             << "|"
             << ownerId           << "\n";
    }
}

void Bank::loadAccounts()
{
    std::ifstream file(ACCOUNTS_FILE);
    if (!file.is_open()) return;   // no file yet — first run

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string iban, type, balStr, extraStr, ownerStr;
        std::getline(ss, iban,     '|');
        std::getline(ss, type,     '|');
        std::getline(ss, balStr,   '|');
        std::getline(ss, extraStr, '|');
        std::getline(ss, ownerStr, '|');

        try {
            double bal   = std::stod(balStr);
            double extra = std::stod(extraStr);
            int ownerId  = std::stoi(ownerStr);

            std::shared_ptr<Account> acc;
            if (type == "savings")
                acc = std::make_shared<SavingsAccount>(iban, bal, extra);
            else if (type == "checking")
                acc = std::make_shared<CheckingAccount>(iban, bal, extra);
            else continue;

            accounts.push_back(acc);

            auto user = findUserById(ownerId);
            if (user) user->addAccount(acc);
        }
        catch (...) {}
    }
}

// ── Print ──────────────────────────────────────────────────────────────────
const std::string& Bank::getName() const { return name; }

void Bank::printUsers() const
{
    for (const auto& u : users) u->print();
}

void Bank::printAccounts() const
{
    for (const auto& a : accounts) a->printInfo();
}

void Bank::printUsersWithAccounts() const
{
    if (users.empty()) { std::cout << "  (no users)\n"; return; }
    for (const auto& u : users)
    {
        std::cout << "  User #" << u->getId() << " — " << u->getName() << "\n";
        const auto& accs = u->getAccounts();
        if (accs.empty()) std::cout << "    (no accounts)\n";
        else for (const auto& a : accs) { std::cout << "    "; a->printInfo(); }
    }
}

void Bank::printTransactions() const
{
    if (transactions.empty()) { std::cout << "  (no transactions)\n"; return; }
    for (const auto& t : transactions) t.print();
}

void Bank::printTransactionsForUser(const std::string& iban) const
{
    bool found = false;
    for (const auto& t : transactions)
    {
        if (t.getFromIBAN() == iban || t.getToIBAN() == iban)
        {
            t.print();
            found = true;
        }
    }
    if (!found) std::cout << "  (no transactions for this IBAN)\n";
}
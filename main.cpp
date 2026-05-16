#include <iostream>
#include <string>
#include <memory>
#include <limits>
#include "AuthManager.h"
#include "MessageSystem.h"
#include "Bank.h"
#include "User.h"
#include "SavingsAccount.h"
#include "CheckingAccount.h"

static void clearInput()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static void printBanner()
{
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════╗\n";
    std::cout << "  ║        Welcome to MyBank     ║\n";
    std::cout << "  ╚══════════════════════════════╝\n\n";
}

// ── Admin session ──────────────────────────────────────────────────────────
static void runAdminSession(Bank& bank, MessageSystem& msgSys)
{
    std::cout << "\n  ╔══════════════════════════════╗\n";
    std::cout << "  ║         Admin Panel          ║\n";
    std::cout << "  ╚══════════════════════════════╝\n";

    int choice = 0;
    while (true)
    {
        std::cout << "\n  [1]  View all users & accounts\n";
        std::cout << "  [2]  View account requests\n";
        std::cout << "  [3]  Approve account request\n";
        std::cout << "  [4]  Deny account request\n";
        std::cout << "  [5]  Delete account\n";
        std::cout << "  [6]  Delete user\n";
        std::cout << "  [7]  View all messages\n";
        std::cout << "  [8]  Reply to a user\n";
        std::cout << "  [9]  View transactions for a user\n";
        std::cout << "  [10] Log out\n";
        std::cout << "  > ";
        std::cin >> choice;

        if (choice == 1)
        {
            std::cout << "\n  -- All Users & Accounts --\n";
            bank.printUsersWithAccounts();
        }
        else if (choice == 2)
        {
            std::cout << "\n  -- Account Requests --\n";
            auto reqs = msgSys.getRequestsForBank();
            if (reqs.empty()) std::cout << "  (no pending requests)\n";
            else for (auto& r : reqs) r.print();
        }
        else if (choice == 3) // Approve
        {
            // Show pending requests first
            auto reqs = msgSys.getRequestsForBank();
            if (reqs.empty()) { std::cout << "  (no pending requests)\n"; continue; }

            std::cout << "\n  -- Pending Requests --\n";
            for (auto& r : reqs) r.print();

            int userId; std::string type, iban;
            double balance, extra;
            std::cout << "\n  Approving for User ID: "; std::cin >> userId;
            std::cout << "  Account type (savings/checking): "; std::cin >> type;
            std::cout << "  Assign IBAN: ";                     std::cin >> iban;
            std::cout << "  Initial balance: ";                 std::cin >> balance;
            std::cout << (type == "savings"
                          ? "  Interest rate (e.g. 0.05): "
                          : "  Overdraft limit: ");
            std::cin >> extra;

            auto acc = bank.createAccountForUser(userId, type, iban, balance, extra);
            if (acc)
            {
                std::cout << "  [+] Account created: " << iban << "\n";
                msgSys.sendMessage(0, "Bank", userId,
                    "Your account request was approved. IBAN: " + iban,
                    MessageType::RequestApproved);
            }
            else
                std::cout << "  [!] Failed — check user ID and account type.\n";
        }
        else if (choice == 4) // Deny
        {
            int userId;
            std::string reason;
            std::cout << "  Deny request for User ID: "; std::cin >> userId;
            clearInput();
            std::cout << "  Reason: "; std::getline(std::cin, reason);
            msgSys.sendMessage(0, "Bank", userId,
                "Your account request was denied. Reason: " + reason,
                MessageType::RequestDenied);
        }
        else if (choice == 5)
        {
            std::string iban;
            std::cout << "  IBAN to delete: "; std::cin >> iban;
            bank.deleteAccount(iban)
                ? std::cout << "  [+] Account deleted.\n"
                : std::cout << "  [!] IBAN not found.\n";
        }
        else if (choice == 6)
        {
            int userId;
            std::cout << "  User ID to delete: "; std::cin >> userId;
            bank.deleteUser(userId)
                ? std::cout << "  [+] User deleted.\n"
                : std::cout << "  [!] User not found.\n";
        }
        else if (choice == 7)
        {
            std::cout << "\n  -- All Messages --\n";
            auto msgs = msgSys.getMessagesForBank();
            if (msgs.empty()) std::cout << "  (no messages)\n";
            else for (auto& m : msgs) m.print();
        }
        else if (choice == 8)
        {
            int toUserId; std::string content;
            std::cout << "  Reply to User ID: "; std::cin >> toUserId;
            clearInput();
            std::cout << "  Message: "; std::getline(std::cin, content);
            msgSys.sendMessage(0, "Bank", toUserId, content, MessageType::Plain);
        }
        else if (choice == 9)
        {
            std::string iban;
            std::cout << "  Enter user IBAN: "; std::cin >> iban;
            std::cout << "\n  -- Transactions for " << iban << " --\n";
            bank.printTransactionsForUser(iban);
        }
        else if (choice == 10)
        {
            std::cout << "  Logging out of admin panel...\n";
            break;
        }
        else
        {
            std::cout << "  [!] Invalid option.\n";
            clearInput();
        }
    }
}

// ── User session ───────────────────────────────────────────────────────────
static void runBankSession(Bank& bank, MessageSystem& msgSys,
                           int userId, const std::string& username)
{
    std::cout << "\n  Logged in as: " << username
              << " (ID: " << userId << ")\n";

    // Register user in bank if first login
    auto user = bank.findUserById(userId);
    if (!user)
    {
        user = std::make_shared<User>(userId, username);
        bank.addUser(user);
        std::cout << "  Profile created. Request an account from the bank to get started.\n";
    }

    int choice = 0;
    while (true)
    {
        std::cout << "\n  --- Bank Menu ---\n";
        std::cout << "  [1] View my accounts\n";
        std::cout << "  [2] Deposit\n";
        std::cout << "  [3] Withdraw\n";
        std::cout << "  [4] Transfer\n";
        std::cout << "  [5] Request new account\n";
        std::cout << "  [6] Send message to bank\n";
        std::cout << "  [7] View messages from bank\n";
        std::cout << "  [8] Log out\n";
        std::cout << "  > ";
        std::cin >> choice;

        if (choice == 1)
        {
            const auto& accs = user->getAccounts();
            if (accs.empty())
                std::cout << "  You have no accounts yet. Request one from the bank.\n";
            else
                user->print();
        }
        else if (choice == 2)
        {
            std::string iban; double amount;
            std::cout << "  IBAN: ";   std::cin >> iban;
            std::cout << "  Amount: "; std::cin >> amount;
            bank.deposit(iban, amount)
                ? std::cout << "  [+] Deposit successful.\n"
                : std::cout << "  [!] Deposit failed.\n";
        }
        else if (choice == 3)
        {
            std::string iban; double amount;
            std::cout << "  IBAN: ";   std::cin >> iban;
            std::cout << "  Amount: "; std::cin >> amount;
            bank.withdraw(iban, amount)
                ? std::cout << "  [+] Withdrawal successful.\n"
                : std::cout << "  [!] Withdrawal failed.\n";
        }
        else if (choice == 4)
        {
            std::string from, to; double amount;
            std::cout << "  From IBAN: "; std::cin >> from;
            std::cout << "  To IBAN:   "; std::cin >> to;
            std::cout << "  Amount:    "; std::cin >> amount;
            bank.transfer(from, to, amount)
                ? std::cout << "  [+] Transfer successful.\n"
                : std::cout << "  [!] Transfer failed.\n";
        }
        else if (choice == 5)
        {
            std::string type, reason;
            std::cout << "  Account type (savings/checking): "; std::cin >> type;
            clearInput();
            std::cout << "  Reason / initial deposit amount: ";
            std::getline(std::cin, reason);

            std::string content = "Account request from " + username
                                + " (ID: " + std::to_string(userId) + ")"
                                + " | Type: " + type
                                + " | Details: " + reason;

            msgSys.sendMessage(userId, username, 0,
                               content, MessageType::AccountRequest);
        }
        else if (choice == 6)
        {
            std::string content;
            clearInput();
            std::cout << "  Message: "; std::getline(std::cin, content);
            msgSys.sendMessage(userId, username, 0, content, MessageType::Plain);
        }
        else if (choice == 7)
        {
            std::cout << "\n  -- Messages from Bank --\n";
            auto msgs = msgSys.getMessagesForUser(userId);
            if (msgs.empty()) std::cout << "  (no messages)\n";
            else for (auto& m : msgs) m.print();
        }
        else if (choice == 8)
        {
            std::cout << "  Logging out...\n";
            break;
        }
        else
        {
            std::cout << "  [!] Invalid option.\n";
            clearInput();
        }
    }
}

// ── main ───────────────────────────────────────────────────────────────────
int main()
{
    Bank bank("MyBank");
    AuthManager auth;
    MessageSystem msgSys;

    printBanner();

    while (true)
    {
        std::cout << "  [1] Log In\n";
        std::cout << "  [2] Sign Up\n";
        std::cout << "  [3] Admin Login\n";
        std::cout << "  [4] Exit\n";
        std::cout << "  > ";

        int choice = 0;
        std::cin >> choice;

        if (choice == 1)
        {
            std::string u, p;
            std::cout << "\n  -- Log In --\n";
            std::cout << "  Username: "; std::cin >> u;
            std::cout << "  Password: "; std::cin >> p;
            AuthResult r = auth.login(u, p);
            if (r.success) runBankSession(bank, msgSys, r.userId, r.username);
        }
        else if (choice == 2)
        {
            std::string u, p;
            std::cout << "\n  -- Sign Up --\n";
            std::cout << "  Username: "; std::cin >> u;
            std::cout << "  Password: "; std::cin >> p;
            AuthResult r = auth.signUp(u, p);
            if (r.success) runBankSession(bank, msgSys, r.userId, r.username);
        }
        else if (choice == 3)
        {
            std::string u, p;
            std::cout << "\n  -- Admin Login --\n";
            std::cout << "  Username: "; std::cin >> u;
            std::cout << "  Password: "; std::cin >> p;
            if (auth.loginAsAdmin(u, p))
                runAdminSession(bank, msgSys);
        }
        else if (choice == 4)
        {
            std::cout << "\n  Goodbye!\n\n";
            break;
        }
        else
        {
            std::cout << "  [!] Invalid option.\n";
            clearInput();
        }
    }

    return 0;
}
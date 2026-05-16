#include <iostream>
#include <memory>
#include "Bank.h"
#include "User.h"          // ← add this
#include "SavingsAccount.h"
#include "CheckingAccount.h"
// ✅ Transaction.h removed — Bank.h already includes it

int main()
{
    Bank bank("MyBank");

    auto user1 = std::make_shared<User>(1, "Ivan Ivanov");
    auto user2 = std::make_shared<User>(2, "Georgi Georgiev");
    bank.addUser(user1);
    bank.addUser(user2);

    auto acc1 = std::make_shared<SavingsAccount>("BG111", 1000.0, 0.05);
    auto acc2 = std::make_shared<CheckingAccount>("BG222", 500.0, 300.0);
    user1->addAccount(acc1);
    user2->addAccount(acc2);
    bank.addAccount(acc1);
    bank.addAccount(acc2);

    std::cout << "\n--- INITIAL STATE ---\n";
    bank.printUsers();
    bank.printAccounts();

    std::cout << "\n--- DEPOSIT 200 -> BG111 ---\n";
    bank.deposit("BG111", 200);

    std::cout << "\n--- WITHDRAW 100 <- BG222 ---\n";
    bank.withdraw("BG222", 100);

    std::cout << "\n--- TRANSFER 300: BG111 -> BG222 ---\n";
    bank.transfer("BG111", "BG222", 300);

    std::cout << "\n--- FINAL STATE ---\n";
    bank.printUsers();
    bank.printAccounts();

    std::cout << "\n--- TRANSACTION HISTORY ---\n";   // ✅ added
    bank.printTransactions();

    return 0;
}
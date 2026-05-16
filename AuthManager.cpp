#include "AuthManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

const std::string AuthManager::USERS_FILE = "users.txt";

bool AuthManager::usernameExists(const std::string& username) const
{
    std::ifstream file(USERS_FILE);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string id, storedUser, pass;
        std::getline(ss, id,   '|');
        std::getline(ss, storedUser, '|');
        std::getline(ss, pass, '|');
        if (storedUser == username) return true;
    }
    return false;
}

int AuthManager::getNextUserId() const
{
    std::ifstream file(USERS_FILE);
    int maxId = 0;
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string idStr;
        std::getline(ss, idStr, '|');
        try { maxId = std::max(maxId, std::stoi(idStr)); }
        catch (...) {}
    }
    return maxId + 1;
}

AuthResult AuthManager::signUp(const std::string& username, const std::string& password)
{
    if (username.empty() || password.empty())
    {
        std::cout << "  [!] Username and password cannot be empty.\n";
        return {false, -1, ""};
    }
    if (usernameExists(username))
    {
        std::cout << "  [!] Username already taken.\n";
        return {false, -1, ""};
    }

    int newId = getNextUserId();
    std::ofstream file(USERS_FILE, std::ios::app);
    if (!file.is_open())
    {
        std::cout << "  [!] Could not open users file.\n";
        return {false, -1, ""};
    }

    file << newId << "|" << username << "|" << password << "\n";
    std::cout << "  [+] Account created successfully! Your user ID is: " << newId << "\n";
    return {true, newId, username};
}

AuthResult AuthManager::login(const std::string& username, const std::string& password)
{
    std::ifstream file(USERS_FILE);
    if (!file.is_open())
    {
        std::cout << "  [!] No users registered yet.\n";
        return {false, -1, ""};
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string idStr, storedUser, storedPass;
        std::getline(ss, idStr,      '|');
        std::getline(ss, storedUser, '|');
        std::getline(ss, storedPass, '|');

        if (storedUser == username && storedPass == password)
        {
            std::cout << "  [+] Welcome back, " << username << "!\n";
            return {true, std::stoi(idStr), username};
        }
    }

    std::cout << "  [!] Invalid username or password.\n";
    return {false, -1, ""};
}
bool AuthManager::loginAsAdmin(const std::string& username,
                                const std::string& password)
{
    // Hardcoded admin credentials — change as needed
    if (username == "admin" && password == "admin123")
    {
        std::cout << "  [+] Admin access granted.\n";
        return true;
    }
    std::cout << "  [!] Invalid admin credentials.\n";
    return false;
}
void AuthManager::loadAllUsersIntoBank(Bank& bank)
{
    std::ifstream file(USERS_FILE);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string idStr, name, pass;
        std::getline(ss, idStr, '|');
        std::getline(ss, name,  '|');
        std::getline(ss, pass,  '|');
        try {
            int id = std::stoi(idStr);
            if (!bank.findUserById(id))
                bank.addUser(std::make_shared<User>(id, name));
        } catch (...) {}
    }
}
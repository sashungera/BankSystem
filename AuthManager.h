#pragma once

#include <string>
#include "Bank.h"

struct AuthResult
{
    bool success;
    int userId;
    std::string username;
};

class AuthManager
{
private:
    static const std::string USERS_FILE;

    bool usernameExists(const std::string& username) const;
    int getNextUserId() const;

public:
    
    void loadAllUsersIntoBank(Bank& bank);
    AuthResult signUp(const std::string& username, const std::string& password);
    AuthResult login(const std::string& username, const std::string& password);
    bool loginAsAdmin(const std::string& username, const std::string& password);
};  
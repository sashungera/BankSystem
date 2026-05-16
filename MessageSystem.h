#pragma once
#include <string>
#include <vector>

enum class MessageType
{
    Plain,
    AccountRequest,
    RequestApproved,
    RequestDenied
};

struct Message
{
    int         id;
    int         fromUserId;   // 0 = bank
    int         toUserId;     // 0 = bank
    std::string senderName;
    std::string content;
    MessageType type;

    void print() const;
};

class MessageSystem
{
private:
    static const std::string MESSAGES_FILE;
    int getNextId() const;

public:
    void sendMessage(int fromUserId, const std::string& senderName,
                     int toUserId,   const std::string& content,
                     MessageType type = MessageType::Plain);

    // messages sent TO the bank (toUserId == 0)
    std::vector<Message> getMessagesForBank() const;

    // account requests only
    std::vector<Message> getRequestsForBank() const;

    // messages sent TO a specific user
    std::vector<Message> getMessagesForUser(int userId) const;
};
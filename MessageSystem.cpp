#include "MessageSystem.h"
#include <fstream>
#include <sstream>
#include <iostream>

const std::string MessageSystem::MESSAGES_FILE = "messages.txt";

int MessageSystem::getNextId() const
{
    std::ifstream file(MESSAGES_FILE);
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

static std::string typeToString(MessageType t)
{
    switch (t)
    {
    case MessageType::Plain:           return "Plain";
    case MessageType::AccountRequest:  return "AccountRequest";
    case MessageType::RequestApproved: return "RequestApproved";
    case MessageType::RequestDenied:   return "RequestDenied";
    default:                           return "Plain";
    }
}

static MessageType typeFromString(const std::string& s)
{
    if (s == "AccountRequest")  return MessageType::AccountRequest;
    if (s == "RequestApproved") return MessageType::RequestApproved;
    if (s == "RequestDenied")   return MessageType::RequestDenied;
    return MessageType::Plain;
}

void MessageSystem::sendMessage(int fromUserId, const std::string& senderName,
                                 int toUserId,   const std::string& content,
                                 MessageType type)
{
    std::ofstream file(MESSAGES_FILE, std::ios::app);
    if (!file.is_open()) { std::cout << "  [!] Could not open messages file.\n"; return; }

    file << getNextId()          << "|"
         << fromUserId           << "|"
         << senderName           << "|"
         << toUserId             << "|"
         << typeToString(type)   << "|"
         << content              << "\n";

    std::cout << "  [+] Message sent.\n";
}

static std::vector<Message> loadAll(const std::string& path)
{
    std::vector<Message> msgs;
    std::ifstream file(path);
    if (!file.is_open()) return msgs;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string idS, fromS, name, toS, typeS, content;
        std::getline(ss, idS,     '|');
        std::getline(ss, fromS,   '|');
        std::getline(ss, name,    '|');
        std::getline(ss, toS,     '|');
        std::getline(ss, typeS,   '|');
        std::getline(ss, content, '|');
        try {
            msgs.push_back({ std::stoi(idS),
                             std::stoi(fromS),
                             std::stoi(toS),
                             name, content,
                             typeFromString(typeS) });
        } catch (...) {}
    }
    return msgs;
}

void Message::print() const
{
    std::string from = (fromUserId == 0) ? "Bank" : senderName;
    std::string to   = (toUserId   == 0) ? "Bank" : "User #" + std::to_string(toUserId);
    std::string tag;
    switch (type)
    {
    case MessageType::AccountRequest:  tag = " [ACCOUNT REQUEST]"; break;
    case MessageType::RequestApproved: tag = " [APPROVED]";        break;
    case MessageType::RequestDenied:   tag = " [DENIED]";          break;
    default:                           tag = "";                    break;
    }
    std::cout << "  [Msg #" << id << "]" << tag
              << " From: " << from << " -> To: " << to
              << "\n    " << content << "\n";
}

std::vector<Message> MessageSystem::getMessagesForBank() const
{
    auto all = loadAll(MESSAGES_FILE);
    std::vector<Message> result;
    for (auto& m : all)
        if (m.toUserId == 0) result.push_back(m);
    return result;
}

std::vector<Message> MessageSystem::getRequestsForBank() const
{
    auto all = loadAll(MESSAGES_FILE);
    std::vector<Message> result;
    for (auto& m : all)
        if (m.toUserId == 0 && m.type == MessageType::AccountRequest)
            result.push_back(m);
    return result;
}

std::vector<Message> MessageSystem::getMessagesForUser(int userId) const
{
    auto all = loadAll(MESSAGES_FILE);
    std::vector<Message> result;
    for (auto& m : all)
        if (m.toUserId == userId) result.push_back(m);
    return result;
}
#ifndef CHATLISTENER_H
#define CHATLISTENER_H

#include <string>
#include <memory>
#include "Client.h"

class ChatServer;

class ChatListener {
private:
    ChatServer* server;

public:
    explicit ChatListener(ChatServer* srv);
    
    void handleMessage(std::shared_ptr<Client> client, const std::string& message);
    
private:
    void handleLogin(std::shared_ptr<Client> client, const std::string& username);
    void handleChatMessage(std::shared_ptr<Client> client, const std::string& message);
    void handleWhoCommand(std::shared_ptr<Client> client);
    void handleDirectMessage(std::shared_ptr<Client> client, const std::string& message);
    void handlePing(std::shared_ptr<Client> client);
    
    std::string trim(const std::string& str);
};

#endif

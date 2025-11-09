#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <winsock2.h>
#include <chrono>
#include <vector>
#include <memory>
#include "serverDefaults.h"

class ChatServer;

class Client
{
protected:
    SOCKET clientSocket;
    std::string username;
    bool authenticated;
    std::chrono::steady_clock::time_point lastActivity;
    ChatServer *server; // The server

public:
    Client(SOCKET socket, ChatServer *srv);
    virtual ~Client();

    SOCKET getSocket() const;
    void close();

    std::string getUsername() const;
    void setUsername(const std::string &name);
    bool isAuthenticated() const;
    void setAuthenticated(bool auth);

    virtual bool sendMessage(const std::string &message);
    std::string receiveMessage();

    void updateActivity();
    bool isIdle(int timeoutSeconds) const;

    ChatServer *getServer() const;
};

#endif

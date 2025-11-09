#ifndef CONNECT_H
#define CONNECT_H

#include "Client.h"
#include "ChatServer.h"
#include <memory>

class Connect {
private:
    std::shared_ptr<Client> client;
    ChatServer* server;
    bool connected;

public:
    Connect(std::shared_ptr<Client> clientObj, ChatServer* srv);
    ~Connect();

    bool establish();
    void disconnect();
    bool isConnected() const;
    
    std::shared_ptr<Client> getClient() const; // Get the underlying client object
    
    bool performHandshake();
};

#endif

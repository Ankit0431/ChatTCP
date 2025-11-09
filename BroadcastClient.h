#ifndef BROADCASTCLIENT_H
#define BROADCASTCLIENT_H

#include "Client.h"
#include <vector>
#include <memory>

class BroadcastClient : public Client
{
public:
    BroadcastClient(SOCKET socket, ChatServer *srv);

    // Broadcast message to all authenticated clients
    void broadcastToAll(const std::string &message);

    // Broadcast message to all except this client
    void broadcastToOthers(const std::string &message);

    // Broadcast a chat message with username
    void broadcastChatMessage(const std::string &message);

    // Broadcast info/notification
    void broadcastInfo(const std::string &info);
};

#endif

#include "BroadcastClient.h"
#include "ChatServer.h"
#include <iostream>

BroadcastClient::BroadcastClient(SOCKET socket, ChatServer *srv)
    : Client(socket, srv)
{
}

// Include broadcast to all authenticated clients just in case.
void BroadcastClient::broadcastToAll(const std::string &message)
{
    if (!server)
        return;

    auto clients = server->getAuthenticatedClients();
    for (auto &client : clients)
    {
        if (client) // Here, we want to broadcast to self as well (to all means including self)
        {
            client->sendMessage(message);
        }
    }
}

void BroadcastClient::broadcastToOthers(const std::string &message)
{
    if (!server)
        return;

    auto clients = server->getAuthenticatedClients();
    for (auto &client : clients)
    {
        if (client && client->getUsername() != this->username) // Here, we will exclude self
        {
            client->sendMessage(message);
        }
    }
}

void BroadcastClient::broadcastChatMessage(const std::string &message)
{
    if (!authenticated)
        return;

    std::string formattedMessage = "MSG " + username + " " + message;
    broadcastToOthers(formattedMessage);
}

void BroadcastClient::broadcastInfo(const std::string &info)
{
    std::string formattedMessage = "INFO " + info;
    broadcastToAll(formattedMessage);
}

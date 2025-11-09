#include "Connect.h"
#include <iostream>

Connect::Connect(std::shared_ptr<Client> clientObj, ChatServer *srv)
    : client(clientObj), server(srv), connected(false)
{
}

Connect::~Connect()
{
    disconnect();
}

bool Connect::establish()
{
    if (!client || !server)
    {
        return false;
    }

    if (client->getSocket() == INVALID_SOCKET)
    {
        return false;
    }

    connected = true;
    return performHandshake();
}

void Connect::disconnect()
{
    if (connected && client)
    {
        client->close();
        connected = false;
    }
}

bool Connect::isConnected() const
{
    return connected;
}

std::shared_ptr<Client> Connect::getClient() const
{
    return client;
}

bool Connect::performHandshake()
{
    // Future: Can add initial handshake protocol here
    // For now, connection is ready for LOGIN command
    return true;
}

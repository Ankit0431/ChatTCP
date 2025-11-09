#include "DMClient.h"
#include "ChatServer.h"
#include <iostream>

DMClient::DMClient(SOCKET socket, ChatServer *srv)
    : Client(socket, srv)
{
}

bool DMClient::sendDirectMessage(const std::string &targetUsername, const std::string &message)
{
    if (!server || !authenticated)
    {
        return false;
    }

    if(targetUsername == this->username)
    {
        return false; // Prevent sending DM to self
    }

    // Attempt to find the target client
    auto targetClient = server->findClientByUsername(targetUsername);

    if (!targetClient)
    {
        return false; // Target user not found
    }
    

    std::string formattedMessage = "DM " + username + " " + message; //Format the message
    return targetClient->sendMessage(formattedMessage);
}

void DMClient::receiveDirectMessage(const std::string &fromUsername, const std::string &message)
{
    std::string formattedMessage = "DM " + fromUsername + " " + message;
    sendMessage(formattedMessage);
}

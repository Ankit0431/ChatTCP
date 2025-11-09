#include "ChatListener.h"
#include "ChatServer.h"
#include "BroadcastClient.h"
#include "DMClient.h"
#include <sstream>
#include <algorithm>

ChatListener::ChatListener(ChatServer *srv) : server(srv) {}

std::string ChatListener::trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void ChatListener::handleMessage(std::shared_ptr<Client> client, const std::string &message)
{
    if (message.empty())
    {
        return;
    }

    std::string trimmedMessage = trim(message);
    std::istringstream iss(trimmedMessage);
    std::string command;
    iss >> command;

    // Convert command to uppercase for case-insensitive comparison
    std::transform(command.begin(), command.end(), command.begin(), ::toupper);

    if (command == "LOGIN")
    {
        std::string username;
        iss >> username;
        handleLogin(client, username);
    }
    else if (command == "PING")
    {
        handlePing(client);
    }
    else if (!client->isAuthenticated())
    {
        client->sendMessage("ERR not-authenticated");
    }
    else if (command == "MSG")
    {
        std::string msg;
        std::getline(iss, msg);
        handleChatMessage(client, trim(msg));
    }
    else if (command == "WHO")
    {
        handleWhoCommand(client);
    }
    else if (command == "DM")
    {
        std::string remainingMessage;
        std::getline(iss, remainingMessage);
        handleDirectMessage(client, trim(remainingMessage));
    }
    else
    {
        client->sendMessage("ERR unknown-command");
    }
}

void ChatListener::handleLogin(std::shared_ptr<Client> client, const std::string &username)
{
    if (client->isAuthenticated())
    {
        client->sendMessage("ERR already-authenticated");
        return;
    }

    if (username.empty())
    {
        client->sendMessage("ERR invalid-username");
        return;
    }

    if (server->isUsernameTaken(username))
    {
        client->sendMessage("ERR username-taken");
        return;
    }

    client->setUsername(username);
    client->setAuthenticated(true);
    client->sendMessage("OK");

    // Notify other users using BroadcastClient instance
    // Use INVALID_SOCKET since we're only using this for broadcasting
    BroadcastClient broadcaster(INVALID_SOCKET, server);
    broadcaster.setUsername(username);
    broadcaster.setAuthenticated(true);
    broadcaster.broadcastInfo(username + " connected");
}

void ChatListener::handleChatMessage(std::shared_ptr<Client> client, const std::string &message)
{
    if (message.empty())
    {
        client->sendMessage("ERR empty-message");
        return;
    }

    // Create BroadcastClient instance and broadcast the message
    // Use INVALID_SOCKET since we're only using this for broadcasting
    BroadcastClient broadcaster(INVALID_SOCKET, server);
    broadcaster.setUsername(client->getUsername());
    broadcaster.setAuthenticated(true);
    broadcaster.broadcastChatMessage(message);
}

void ChatListener::handleWhoCommand(std::shared_ptr<Client> client)
{
    auto clients = server->getAuthenticatedClients();
    for (auto &c : clients)
    {
        if (c)
        {
            client->sendMessage("USER " + c->getUsername());
        }
    }
}

void ChatListener::handleDirectMessage(std::shared_ptr<Client> client, const std::string &message)
{
    std::istringstream iss(message);
    std::string targetUsername;
    iss >> targetUsername;

    if (targetUsername.empty())
    {
        client->sendMessage("ERR invalid-dm-format");
        return;
    }

    std::string dmMessage;
    std::getline(iss, dmMessage);
    dmMessage = trim(dmMessage);

    if (dmMessage.empty())
    {
        client->sendMessage("ERR empty-message");
        return;
    }

    // Create DMClient instance and send direct message
    // Use INVALID_SOCKET since we're only using this for sending DM
    DMClient dmSender(INVALID_SOCKET, server);
    dmSender.setUsername(client->getUsername());
    dmSender.setAuthenticated(true);

    if (!dmSender.sendDirectMessage(targetUsername, dmMessage))
    {
        client->sendMessage("ERR user-not-found");
    }
}
void ChatListener::handlePing(std::shared_ptr<Client> client)
{
    client->sendMessage("PONG");
}

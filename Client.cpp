#include "Client.h"
#include <iostream>
#include <ws2tcpip.h>


Client::Client(SOCKET socket, ChatServer *srv)
    : clientSocket(socket), authenticated(false), username(""), server(srv)
{
    updateActivity();
}

Client::~Client()
{
    close();
}

SOCKET Client::getSocket() const
{
    return clientSocket;
}

std::string Client::getUsername() const
{
    return username;
}

void Client::setUsername(const std::string &name)
{
    username = name;
}

bool Client::isAuthenticated() const
{
    return authenticated;
}

void Client::setAuthenticated(bool auth)
{
    authenticated = auth;
}

bool Client::sendMessage(const std::string &message)
{
    std::string fullMessage = message + "\n";
    if(fullMessage.length() > MAX_BUFFER_SIZE)
    {
        std::cerr << "Message too long" << std::endl;
        return false; // Message too long
    }
    int result = send(clientSocket, fullMessage.c_str(), fullMessage.length(), 0);
    return result != SOCKET_ERROR;
}

std::string Client::receiveMessage()
{
    char buffer[MAX_BUFFER_SIZE + 1];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived > 0)
    {
        buffer[bytesReceived] = '\0';
        updateActivity();
        std::string message(buffer);

        // Remove trailing newline/carriage return
        while (!message.empty() && (message.back() == '\n' || message.back() == '\r'))
        {
            message.pop_back();
        }

        return message;
    }

    return "";
}

void Client::updateActivity()
{
    lastActivity = std::chrono::steady_clock::now();
}

bool Client::isIdle(int timeoutSeconds) const
{
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastActivity);
    return duration.count() >= timeoutSeconds;
}

void Client::close()
{
    if (clientSocket != INVALID_SOCKET)
    {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }
}

ChatServer *Client::getServer() const
{
    return server;
}

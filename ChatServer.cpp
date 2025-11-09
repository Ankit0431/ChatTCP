#include "ChatServer.h"
#include "ChatListener.h"
#include "BroadcastClient.h"
#include <iostream>
#include <thread>
#include <algorithm>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

ChatServer::ChatServer(int serverPort, int idleTimeout)
    : port(serverPort), serverSocket(INVALID_SOCKET), running(false),
      idleTimeoutSeconds(idleTimeout)
{
    listener = std::make_unique<ChatListener>(this);
}

ChatServer::~ChatServer()
{
    stop();
}

bool ChatServer::initializeWinsock()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }
    return true;
}

void ChatServer::cleanupWinsock()
{
    WSACleanup();
}

bool ChatServer::initialize()
{
    if (!initializeWinsock())
    {
        return false;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        cleanupWinsock();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        cleanupWinsock();
        return false;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        cleanupWinsock();
        return false;
    }

    std::cout << "Server initialized on port " << port << std::endl;
    return true;
}

void ChatServer::start()
{
    running = true;
    std::cout << "Server started. Waiting for connections..." << std::endl;

    // Start idle checker thread
    std::thread idleCheckerThread([this]()
                                  {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            checkIdleClients();
        } });
    idleCheckerThread.detach();

    acceptClients();
}

void ChatServer::acceptClients()
{
    while (running)
    {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET)
        {
            if (running)
            {
                std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            }
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        std::cout << "New connection from " << clientIP << std::endl;

        // Create a new Client object for this connection
        auto client = std::make_shared<Client>(clientSocket, this);

        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back(client);
        }

        // Handle client in a new thread
        std::thread clientThread(&ChatServer::handleClient, this, client);
        clientThread.detach();
    }
}

void ChatServer::handleClient(std::shared_ptr<Client> client)
{
    try
    {
        while (running && client->getSocket() != INVALID_SOCKET)
        {
            std::string message = client->receiveMessage();

            if (message.empty())
            {
                // Client disconnected
                break;
            }

            listener->handleMessage(client, message);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception handling client: " << e.what() << std::endl;
    }

    // Client disconnected
    if (client->isAuthenticated())
    {
        std::cout << "User " << client->getUsername() << " disconnected" << std::endl;

        // Broadcast disconnection using BroadcastClient instance
        // Use INVALID_SOCKET since we're only using this for broadcasting
        BroadcastClient broadcaster(INVALID_SOCKET, this);
        broadcaster.setUsername(client->getUsername());
        broadcaster.setAuthenticated(true);
        broadcaster.broadcastInfo(client->getUsername() + " disconnected");
    }

    removeClient(client);
}

void ChatServer::removeClient(std::shared_ptr<Client> client)
{
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(
        std::remove_if(clients.begin(), clients.end(),
                       [&client](const std::shared_ptr<Client> &c)
                       {
                           return c->getSocket() == client->getSocket();
                       }),
        clients.end());
}

void ChatServer::checkIdleClients()
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (auto it = clients.begin(); it != clients.end();)
    {
        auto &client = *it;

        if (client->isAuthenticated() && client->isIdle(idleTimeoutSeconds))
        {
            std::cout << "User " << client->getUsername() << " timed out due to inactivity" << std::endl;
            client->sendMessage("INFO timeout-disconnect");

            std::string username = client->getUsername();

            // Notify other users using BroadcastClient instance
            // Use INVALID_SOCKET since we're only using this for broadcasting
            BroadcastClient broadcaster(INVALID_SOCKET, this);
            broadcaster.setUsername(username);
            broadcaster.setAuthenticated(true);
            broadcaster.broadcastInfo(username + " disconnected (timeout)");

            client->close();
            it = clients.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
bool ChatServer::isUsernameTaken(const std::string &username)
{
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto &client : clients)
    {
        if (client->isAuthenticated() && client->getUsername() == username)
        {
            return true;
        }
    }
    return false;
}

std::vector<std::shared_ptr<Client>> ChatServer::getClients()
{
    std::lock_guard<std::mutex> lock(clientsMutex);
    return clients;
}

std::vector<std::shared_ptr<Client>> ChatServer::getAuthenticatedClients()
{
    std::lock_guard<std::mutex> lock(clientsMutex);
    std::vector<std::shared_ptr<Client>> authClients;
    for (auto &client : clients)
    {
        if (client->isAuthenticated())
        {
            authClients.push_back(client);
        }
    }
    return authClients;
}

std::shared_ptr<Client> ChatServer::findClientByUsername(const std::string &username)
{
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto &client : clients)
    {
        if (client->isAuthenticated() && client->getUsername() == username)
        {
            return client;
        }
    }
    return nullptr;
}

void ChatServer::stop()
{
    if (!running)
    {
        return;
    }

    running = false;
    std::cout << "Shutting down server..." << std::endl;

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        for (auto &client : clients)
        {
            client->sendMessage("INFO server-shutdown");
            client->close();
        }
        clients.clear();
    }

    if (serverSocket != INVALID_SOCKET)
    {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    cleanupWinsock();
    std::cout << "Server stopped" << std::endl;
}

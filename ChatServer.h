#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <winsock2.h>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include "Client.h"

class ChatListener;

class ChatServer
{
private:
    int port;
    SOCKET serverSocket;
    std::vector<std::shared_ptr<Client>> clients;
    std::mutex clientsMutex;
    std::atomic<bool> running;
    std::unique_ptr<ChatListener> listener;
    int idleTimeoutSeconds;

public:
    explicit ChatServer(int serverPort = 4000, int idleTimeout = 60);
    ~ChatServer();

    bool initialize();
    void start();
    void stop();

    bool isUsernameTaken(const std::string &username);
    std::vector<std::shared_ptr<Client>> getClients();
    void removeClient(std::shared_ptr<Client> client);

    std::vector<std::shared_ptr<Client>> getAuthenticatedClients(); // Get all authenticated clients

    std::shared_ptr<Client> findClientByUsername(const std::string &username); 

private:
    void acceptClients();
    void handleClient(std::shared_ptr<Client> client);
    void checkIdleClients();

    static bool initializeWinsock();
    static void cleanupWinsock();
};

#endif

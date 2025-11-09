#include "ChatServer.h"
#include <iostream>
#include <cstdlib>
#include <csignal>
#include "serverDefaults.h"

ChatServer* globalServer = nullptr;

void signalHandler(int signal) {
    std::cout << "\nReceived interrupt signal. Shutting down..." << std::endl;
    if (globalServer) {
        globalServer->stop();
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;
    int idleTimeout = DEFAULT_IDLE_TIMEOUT; 

    // Check for port from CLI
    if (argc >= 2) {
        port = std::atoi(argv[1]);
    }

    // Check for idle timeout from CLI
    if (argc >= 3) {
        idleTimeout = std::atoi(argv[2]);
    }

    std::cout << "========================================" << std::endl;
    std::cout << "   TCP Chat Server (OOP Architecture)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Port: " << port << std::endl;
    std::cout << "Idle Timeout: " << idleTimeout << " seconds" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nArchitecture:" << std::endl;
    std::cout << "  - ChatServer: Main server (always active)" << std::endl;
    std::cout << "  - Client: Base class for each connection" << std::endl;
    std::cout << "  - BroadcastClient: Handles message broadcasting" << std::endl;
    std::cout << "  - DMClient: Handles direct messages" << std::endl;
    std::cout << "  - Connect: Manages client-server connection" << std::endl;
    std::cout << "========================================" << std::endl;

    ChatServer server(port, idleTimeout);
    globalServer = &server;

    // Set up signal handler for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    if (!server.initialize()) {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }

    std::cout << "\nProtocol Commands:" << std::endl;
    std::cout << "  LOGIN <username>   - Log in with a username" << std::endl;
    std::cout << "  MSG <text>         - Send a message to all users" << std::endl;
    std::cout << "  DM <user> <text>   - Send a direct message" << std::endl;
    std::cout << "  WHO                - List all connected users" << std::endl;
    std::cout << "  PING               - Keep connection alive (server responds with PONG)" << std::endl;
    std::cout << "\nPress Ctrl+C to stop the server\n" << std::endl;

    server.start();

    return 0;
}

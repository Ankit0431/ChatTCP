#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#define MAX_BUFFER_SIZE 1024

#pragma comment(lib, "ws2_32.lib")

class ChatClient
{
private:
    SOCKET clientSocket;
    std::string serverAddress;
    int serverPort;
    bool connected;

public:
    ChatClient(const std::string &address, int port)
        : serverAddress(address), serverPort(port),
          clientSocket(INVALID_SOCKET), connected(false) {}

    ~ChatClient()
    {
        disconnect();
    }

    bool connect()
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "WSAStartup failed" << std::endl;
            return false;
        }

        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Socket creation failed" << std::endl;
            WSACleanup();
            return false;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);

        if (::connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return false;
        }

        connected = true;
        std::cout << "Connected to server at " << serverAddress << ":" << serverPort << std::endl;
        return true;
    }

    void disconnect()
    {
        if (clientSocket != INVALID_SOCKET)
        {
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
        }
        connected = false;
        WSACleanup();
    }

    bool sendMessage(const std::string &message)
    {
        if (!connected)
            return false;

        std::string fullMessage = message + "\n";
        if(fullMessage.length() > MAX_BUFFER_SIZE)
        {
            std::cerr << "Message too long" << std::endl;
            return false; // Message too long
        }
        int result = send(clientSocket, fullMessage.c_str(), fullMessage.length(), 0);
        return result != SOCKET_ERROR;
    }

    std::string receiveMessage()
    {
        if (!connected)
            return "";

        char buffer[MAX_BUFFER_SIZE + 1];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0';
            std::string message(buffer);

            // Remove trailing newline
            while (!message.empty() && (message.back() == '\n' || message.back() == '\r'))
            {
                message.pop_back();
            }

            return message;
        }

        return "";
    }

    void receiveLoop()
    {
        while (connected)
        {
            std::string message = receiveMessage();
            if (!message.empty())
            {
                std::cout << "< " << message << std::endl;
            }
            else if (connected)
            {
                std::cout << "Disconnected from server" << std::endl;
                connected = false;
                break;
            }
        }
    }

    bool isConnected() const
    {
        return connected;
    }
};

int main(int argc, char *argv[])
{
    std::string serverAddress = "127.0.0.1";
    int serverPort = 4000;

    if (argc >= 2)
    {
        serverAddress = argv[1];
    }
    if (argc >= 3)
    {
        serverPort = std::atoi(argv[2]);
    }

    std::cout << "========================================" << std::endl;
    std::cout << "   TCP Chat Client" << std::endl;
    std::cout << "========================================" << std::endl;

    ChatClient client(serverAddress, serverPort);

    if (!client.connect())
    {
        return 1;
    }

    std::cout << "\nCommands:" << std::endl;
    std::cout << "  LOGIN <username>   - Log in" << std::endl;
    std::cout << "  MSG <text>         - Send message to all" << std::endl;
    std::cout << "  DM <user> <text>   - Send direct message" << std::endl;
    std::cout << "  WHO                - List users" << std::endl;
    std::cout << "  PING               - Ping server" << std::endl;
    std::cout << "  quit               - Exit\n"
              << std::endl;

    // Start receiver thread
    std::thread receiveThread(&ChatClient::receiveLoop, &client);
    receiveThread.detach();

    // Main input loop
    std::string input;
    while (client.isConnected())
    {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "quit" || input == "exit")
        {
            break;
        }

        if (!input.empty())
        {
            if (!client.sendMessage(input))
            {
                std::cerr << "Failed to send message" << std::endl;
                break;
            }
        }
    }

    client.disconnect();
    std::cout << "Goodbye!" << std::endl;

    return 0;
}

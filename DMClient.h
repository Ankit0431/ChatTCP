#include "Client.h"
#include <string>

class DMClient : public Client
{
public:
    DMClient(SOCKET socket, ChatServer *srv);

    bool sendDirectMessage(const std::string &targetUsername, const std::string &message);

    void receiveDirectMessage(const std::string &fromUsername, const std::string &message);
};



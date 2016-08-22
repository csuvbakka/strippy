#pragma once

#include <string>

class ClientSocket
{
public:
    ClientSocket();
    ~ClientSocket();
    ClientSocket(const ClientSocket&) = delete;

    bool connect(const std::string& host, int port);

    // private:
    int sockfd_;
};

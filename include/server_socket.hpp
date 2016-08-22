#pragma once

class ServerSocket
{
public:
    bool bind(int port);
    bool listen();
    int accept();

private:
    int sockfd_;
};

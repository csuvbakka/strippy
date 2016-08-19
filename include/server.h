#pragma once

#include "server_socket.h"
#include <future>

template <typename ChildThread> class Server
{
public:
    Server() {}

    void start(int port)
    {
        socket_.bind(port);
        socket_.listen();
        while (1)
        {
            int client_fd = socket_.accept();
            if (client_fd != -1)
                std::async(std::launch::async, child_thread_, client_fd);
        }
    }

private:
    ServerSocket socket_;
    ChildThread child_thread_;
};

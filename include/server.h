#pragma once

#include "server_socket.h"
// #include <future>
#include <thread>
#include <iostream>

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
                std::thread([this, client_fd]()
                            {
                                child_thread_(client_fd);
                            }).detach();
        }
    }

private:
    ServerSocket socket_;
    ChildThread child_thread_;
};

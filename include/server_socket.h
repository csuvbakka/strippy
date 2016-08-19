#pragma once

class Callable
{
public:
    virtual ~Callable() {}
    virtual void operator()(int) = 0;
};

class ServerSocket
{
public:
    ServerSocket();

    bool bind();
    bool listen();
    void accept(Callable&& c);

private:
    int sockfd_;
};

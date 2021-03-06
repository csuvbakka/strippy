#include <server_socket.hpp>

#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <iostream>
#include <string>

namespace
{
void* get_in_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
}

bool ServerSocket::bind(int port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    struct addrinfo* servinfo;
    int rv;
    if ((rv = getaddrinfo(NULL, std::to_string(port).c_str(), &hints,
                          &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return false;
    }

    // loop through all the results and bind to the first we can
    int sockfd;
    struct addrinfo* p;
    int yes = 1;
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
            -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
            -1)
        {
            perror("setsockopt");
            return false;
        }
        // if (setsockopt(sockfd, SOL_TCP, TCP_NODELAY, &yes, sizeof(int)) ==
        // -1)
        // {
        // perror("setsockopt");
        // return false;
        // }

        if (::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        return false;
    }

    sockfd_ = sockfd;
    return true;
}

bool ServerSocket::listen()
{
    std::cout << "Listening\n";
    // how many pending connections queue will hold
    static const int backlog = 10;
    if (::listen(sockfd_, backlog) == -1)
    {
        perror("listen");
        return false;
    }

    return true;
}

int ServerSocket::accept()
{
    char s[INET6_ADDRSTRLEN];
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size = sizeof their_addr;
    int new_fd = ::accept(sockfd_, (struct sockaddr*)&their_addr, &sin_size);
    if (new_fd != -1)
    {
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr*)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);
    }
    else
        perror("accept");

    return new_fd;
}

#include <strippy_socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BACKLOG 10     // how many pending connections queue will hold

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void child_loop(int client_fd)
{
    if (send(client_fd, "Hello, world!", 13, 0) == -1)
        perror("send");

    close(client_fd);
    exit(0);
}

int main()
{
    int server_socket = create_server_socket("4999", AF_INET);
    server_listen(server_socket, BACKLOG);

    printf("server: waiting for connections...\n");

    socklen_t sin_size;
    struct sockaddr_storage their_addr; // connector's address information
    int new_fd;
    char s[INET6_ADDRSTRLEN];
    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(server_socket, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(server_socket); // child doesn't need the listener
            child_loop(new_fd);
        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}

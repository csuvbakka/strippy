#pragma once

int create_server_socket(const char* service, int addr_family);
void server_listen(int sockfd, int backlog);

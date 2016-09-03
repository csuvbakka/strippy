#pragma once

// #include <experimental/optional>
#include <http_request.hpp>
#include <http_response.hpp>

namespace http
{
int receive_request(int client_fd, mystr::MyStringBuffer& buffer,
                    http::Request& request);
// http::Request receive_request(int client_fd, mystr::MyStringBuffer& buffer);
int receive_response(int client_fd, mystr::MyStringBuffer& buffer,
                     http::Request& response);
// http::Request receive_response(int client_fd, mystr::MyStringBuffer& buffer);
// std::experimental::optional<http::Response> receive_response(int client_fd);
}

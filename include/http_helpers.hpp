#pragma once

// #include <experimental/optional>
#include <http_request.hpp>
#include <http_response.hpp>

namespace http
{
http::Request receive_request(int client_fd, mystr::MyStringBuffer& buffer);
// std::experimental::optional<http::Response> receive_response(int client_fd);
}

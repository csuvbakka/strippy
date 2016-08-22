#include <http_helpers.hpp>
#include <timer.hpp>
#include <socket_functions.hpp>
#include <string_utils.hpp>

#include <iostream>
#include <thread>
#include <chrono>

namespace http
{
std::experimental::optional<http::Request> receive_request(int client_fd)
{
    std::string message = {};
    Timer timer(5000);
    do
    {
        message += util::socket::recv_no_wait(client_fd);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        timer.reset();
    } while (!util::string::ends_with(message, "\r\n\r\n") &&
             !timer.is_timed_out());

    if (timer.is_timed_out())
        return {};
    else
        return http::Request(message);
}

// std::experimental::optional<http::Response> receive_response(int client_fd)
// {
// std::string message = {};
// std::size_t content_length = 0, headers_length;
// while (1)
// {
// message += util::socket::recv(client_fd);
// if (content_length > 0)
// {
// std::cout << "until: " << content_length + headers_length
// << std::endl;
// std::cout << "actual: " << message.length() << std::endl;
// std::cout << message << std::endl;
// if (message.length() >= headers_length + content_length)
// break;
// }
// else
// {
// auto headers_end_pos = message.find("\r\n\r\n");
// if (headers_end_pos != std::string::npos)
// {
// headers_length = headers_end_pos + 4;
// const std::string to_find = "Content-Length:";
// auto pos = message.find(to_find);
// if (pos != std::string::npos)
// {
// std::cout << message.substr(pos, to_find.length() + 10)
// << std::endl;
// auto start = pos + to_find.length();
// auto end = message.find("\r\n", start);
// content_length = std::stoi(util::string::ltrim(
// message.substr(start, end - start)));
// std::cout << "content length: " << content_length
// << std::endl;
// message.reserve(content_length + headers_length);
// }
// }
// }
// std::this_thread::sleep_for(std::chrono::milliseconds(1));
// }
// std::cout << "headers length: " << headers_length << std::endl;
// std::cout << "content length: " << content_length << std::endl;

// return http::Response(message);
// }

std::experimental::optional<http::Response> receive_response(int client_fd)
{
    std::string message = {}, buffer = {};
    buffer.reserve(1024);
    message.reserve(1024);
    do
    {
        std::cout << "filling buffer" << std::endl;
        buffer = util::socket::recv(client_fd);
        std::cout << "got " << buffer << std::endl;
        message += buffer;
    } while (!buffer.empty() && !util::string::ends_with(buffer, "\r\n\r\n"));
    // std::cout << message << std::endl;

    return http::Response(message);
}
}

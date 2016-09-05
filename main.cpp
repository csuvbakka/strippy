#include <server.hpp>
#include <socket_functions.hpp>

#include <unistd.h>
#include <utility>
#include <functional>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <iterator>
#include <experimental/optional>

#include <http_request.hpp>
#include <http_response.hpp>
#include <timer.hpp>
#include <client_socket.hpp>
#include <string_utils.hpp>
#include <http_helpers.hpp>
#include <http_message.hpp>
#include <buffer_reader.hpp>
#include <receive_buffer.hpp>
#include <http_message_body.hpp>

// struct ChildThread
// {
// void operator()(int client_fd)
// {
// while (1)
// {
// str::character_array buffer;
// http::Request message(buffer);
// auto bytes = http::receive_request(client_fd, buffer, message);
// if (bytes == 0)
// break;

// std::cout << client_fd << " Victim => Proxy | "
// << message.request_line() << std::endl;

// const std::string& host = message["Host"];
// ClientSocket client;
// if (!client.connect(host, 80))
// std::cout << "failed to connect" << std::endl;
// else
// {
// // std::cout << "sending to " << host << ": " <<
// // message.data()
// // << std::endl;
// auto sent = util::socket::send(client.sockfd_, message.data());
// std::cout << client_fd << " Proxy => Host | " << sent
// << std::endl;

// str::character_array recv_buffer;
// http::Request response(recv_buffer);
// http::receive_request(client.sockfd_, recv_buffer, response);
// if (response && response.content_length() > 0)
// {
// util::socket::send(client_fd, response.data());
// const auto content_length = response.content_length();
// const auto buf_len = 1048576;
// // const auto buf_len = 2048;
// char buf[buf_len] = {};
// std::size_t sum = 0;
// if (!response.content().empty())
// sum += response.content().length();

// do
// {
// bytes =
// util::socket::recvv(client.sockfd_, buf, buf_len);
// if (bytes > 0)
// {

// std::cout << client_fd << " Proxy <= Host | ";
// std::cout << bytes << std::endl;
// sent = util::socket::send(client_fd, buf, bytes);
// std::cout << client_fd << "Victim <= Proxy | "
// << sent << std::endl;
// sum += sent;
// }
// std::this_thread::sleep_for(
// std::chrono::milliseconds(1));
// } while (bytes != 0 && !(sum >= content_length));
// }
// }

// if (message["Connection"] == "close")
// break;

// std::this_thread::sleep_for(std::chrono::milliseconds(1));
// }

// std::cout << client_fd << " Closing " << std::endl;
// close(client_fd);
// }
// };
struct ChildThread
{
    using char_array_receiver =
        util::socket::Receiver<str::character_array, 1024>;
    // using string_receiver = util::socket::Receiver<std::string, 2048>;
    using string_receiver = util::socket::Receiver<std::string, 1048576>;

    using header_reader = str::BufferReader<char_array_receiver>;
    using body_reader = str::BufferReader<string_receiver>;

    using message_type = http::Message<header_reader>;
    using message_body_type = http::MessageBody<body_reader>;

    void operator()(int client_fd)
    {
        std::cout << "new connection " << client_fd << std::endl;
        while (1)
        {

            str::character_array request_buffer;

            header_reader request_reader(client_fd, request_buffer);
            message_type request(request_reader);

            if (!receive_message_headers(request))
                return;

            std::string request_body_buffer(request_buffer.str());
            body_reader request_body_reader(client_fd, request_body_buffer);
            receive_message_data(request, request_body_reader);

            ClientSocket client;
            if (!forward_request(client, request["Host"], request_buffer.data(),
                                 request_body_buffer))
                return;

            str::character_array response_buffer;

            header_reader response_reader(client.sockfd_, response_buffer);
            message_type response(response_reader);

            if (!receive_message_headers(response))
                return;

            std::string response_body_buffer(response_buffer.str());
            body_reader response_body_reader(client.sockfd_,
                                             response_body_buffer);
            receive_message_data(response, response_body_reader);

            forward_response(client_fd, response_buffer.data(),
                             response_body_buffer);

            if (request["Connection"] == "close")
                break;
        }

        std::cout << "Closing " << client_fd << std::endl;
        close(client_fd);
    }

    bool receive_request(header_reader& request_reader,
                         body_reader& request_body_reader, std::string& host)
    {
        std::cout << "Receiving request headers" << std::endl;
        message_type request(request_reader);
        request.receive();
        if (!request)
        {
            std::cout << "failed to receive" << std::endl;
            return false;
        }

        std::cout << "Receiving request body" << std::endl;
        message_body_type request_body(request, request_body_reader);
        request_body.receive();

        host = request["Host"];
        return true;
    }

    bool forward_request(ClientSocket& client, const std::string& host,
                         const std::string& header_data,
                         const std::string& body_data)
    {
        std::cout << "Forwarding request:" << std::endl;
        if (!client.connect(host, 80))
        {
            std::cout << "failed to connect" << std::endl;
            return false;
        }
        std::cout << "Connected" << std::endl;
        auto sent = util::socket::send(client.sockfd_, header_data);
        if (!body_data.empty())
            sent = util::socket::send(client.sockfd_, body_data);
        std::cout << "Sent Request to " << host << std::endl;

        return true;
    }

    bool receive_message_headers(message_type& message)
    {
        std::cout << "Receiving message headers" << std::endl;
        message.receive();
        if (!message)
        {
            std::cout << "Failed to receive message" << std::endl;
            return false;
        }

        return true;
    }
    void receive_message_data(message_type& message,
                              body_reader& message_body_reader)
    {
        std::cout << "Receiving message body" << std::endl;
        message_body_type message_body(message, message_body_reader);

        message_body.receive();
        std::cout << "Received bytes: " << message_body.content_length()
                  << std::endl;
    }

    void forward_response(int socket_fd, const std::string& header_data,
                          const std::string& body_data)
    {
        std::cout << "Forwarding response headers" << std::endl;
        auto sent = util::socket::send(socket_fd, header_data.data());
        if (!body_data.empty())
        {
            std::cout << "Forwarding response data bytes: "
                      << body_data.length() << std::endl;
            sent = util::socket::send(socket_fd, body_data);
        }
    }
};

int main()
{
    Server<ChildThread> s;
    s.start(4999);
    return 0;
}

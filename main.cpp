#include <server.hpp>

#include <client_socket.hpp>
#include <http_message.hpp>
#include <buffer_reader.hpp>
#include <receive_buffer.hpp>
#include <http_message_body.hpp>

#include <socket_functions.hpp>
#include <unistd.h>

struct ChildThread
{
    using char_array_receiver =
        util::socket::Receiver<str::character_array, 1024>;
    // using string_receiver = util::socket::Receiver<std::string, 2048>;
    using string_receiver = util::socket::Receiver<std::string, 8192>;
    // using string_receiver = util::socket::Receiver<std::string, 1048576>;

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
                break;

            std::string request_body_buffer(request_buffer.str());
            body_reader request_body_reader(client_fd, request_body_buffer);
            receive_message_data(request, request_body_reader);

            ClientSocket client;
            if (!forward_request(client, request["Host"], request_buffer.data(),
                                 request_body_buffer))
                break;

            str::character_array response_buffer;

            header_reader response_reader(client.sockfd_, response_buffer);
            message_type response(response_reader);

            if (!receive_message_headers(response))
                break;

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

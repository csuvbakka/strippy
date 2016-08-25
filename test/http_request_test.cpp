#include <gtest/gtest.h>
#include <http_request.hpp>

TEST(HttpRequestTest, first_line_is_the_request_line)
{
    http::Request request;
    std::string request_line = "GET /path/to/file/index.html HTTP/1.0";
    request >> request_line >> "\r\n" >> "something";

    EXPECT_EQ(request_line, request.request_line());
}

TEST(HttpRequestTest, parse_one_header)
{
    http::Request request;
    std::string request_line = "GET /path/to/file/index.html HTTP/1.0";
    std::string host_header_string = "Host";
    std::string host_data = "127.0.0.1";
    std::string host_header = host_header_string + ": " + host_data;
    request >> request_line >> "\r\n" >> host_header >> "\r\n";

    EXPECT_EQ(host_data, request["Host"]);

    http::Request request_2;
    request_2 >> request_line + "\r\n" + host_header + "\r\n";

    EXPECT_EQ(host_data, request_2["Host"]);
}

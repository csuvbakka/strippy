#include <gtest/gtest.h>
#include <http_request.hpp>

namespace
{
const std::string HOST = "Host";
const std::string REFERER = "Referer";
const std::string CRLF = "\r\n";

const std::string REQUEST_LINE = "GET /path/to/file/index.html HTTP/1.0";
}

TEST(HttpRequestTest, first_line_is_the_request_line)
{
    http::Request request;
    request >> REQUEST_LINE >> CRLF >> "something";

    EXPECT_EQ(REQUEST_LINE, request.request_line());
}

TEST(HttpRequestTest, parse_one_header)
{
    http::Request request;
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ": " + host_data;
    request >> REQUEST_LINE >> CRLF >> host_header >> CRLF;

    EXPECT_EQ(host_data, request[HOST]);

    http::Request request_2;
    request_2 >> REQUEST_LINE + CRLF + host_header + CRLF;

    EXPECT_EQ(host_data, request_2[HOST]);
}

TEST(HttpRequestTest, parse_two_headers)
{
    http::Request request;
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ": " + host_data;

    std::string referer_data = "http://en.wikipedia.org/wiki/Main_Page";
    std::string referer_header = REFERER + ":" + referer_data;
    request >> REQUEST_LINE >> CRLF >> host_header >> CRLF >> referer_header >>
        CRLF + CRLF;

    EXPECT_EQ(host_data, request[HOST]);
    EXPECT_EQ(referer_data, request[REFERER]);

    http::Request request_2;
    request_2 >>
        (REQUEST_LINE + CRLF + host_header + CRLF + referer_header + CRLF);

    EXPECT_EQ(host_data, request_2["Host"]);
    EXPECT_EQ(referer_data, request_2[REFERER]);
}

TEST(HttpRequestTest, parse_two_headers_2)
{
    http::Request request;
    std::string host_header_string = "Host";
    std::string host_data = "127.0.0.1";
    std::string host_header = host_header_string + ": " + host_data;

    std::string referer_data = "http://en.wikipedia.org/wiki/Main_Page";
    std::string referer_header = REFERER + ":" + referer_data;
    request >> REQUEST_LINE >> CRLF >> host_header >> CRLF >> referer_header;

    EXPECT_EQ(host_data, request[HOST]);
    EXPECT_EQ("", request[REFERER]);

    http::Request request_2;
    request_2 >>
        (REQUEST_LINE + CRLF + host_header + CRLF + referer_header + CRLF);

    EXPECT_EQ(host_data, request_2[HOST]);
    EXPECT_EQ(referer_data, request_2[REFERER]);
}

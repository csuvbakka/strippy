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

TEST(HttpRequestTest, parse_request_line_in_multiple_chunks)
{
    http::Request request;
    std::string request_line_first_half = "GET /path/to/fi";
    std::string request_line_second_half = "le/index.html HTTP/1.0";
    request >> request_line_first_half;
    EXPECT_EQ("", request.request_line());

    request >> request_line_second_half;
    EXPECT_EQ("", request.request_line());

    request >> CRLF;
    EXPECT_EQ(request_line_first_half + request_line_second_half,
              request.request_line());
}

TEST(HttpRequestTest, parse_one_header)
{
    http::Request request;
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ": " + host_data;
    request >> REQUEST_LINE >> CRLF >> host_header >> CRLF;

    EXPECT_EQ(host_data, request[HOST]);

    http::Request request_full;
    request_full >> REQUEST_LINE + "\n" + host_header + CRLF;

    EXPECT_EQ(REQUEST_LINE, request_full.request_line());
    EXPECT_EQ(host_data, request_full[HOST]);
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

    http::Request request_full;
    request_full >>
        (REQUEST_LINE + CRLF + host_header + CRLF + referer_header + CRLF);

    EXPECT_EQ(host_data, request_full[HOST]);
    EXPECT_EQ(referer_data, request_full[REFERER]);
}

TEST(HttpRequestTest, parse_two_headers_small_chunks)
{
    http::Request request;
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ": " + host_data;

    std::string referer_data = "http://en.wikipedia.org/wiki/Main_Page";
    std::string referer_header = REFERER + ":" + referer_data;

    request >> REQUEST_LINE >> CRLF >> host_header;
    EXPECT_EQ("", request[HOST]);

    request >> CRLF;
    EXPECT_EQ(host_data, request[HOST]);

    request >> referer_header;
    EXPECT_EQ("", request[REFERER]);

    request >> CRLF;
    EXPECT_EQ(referer_data, request[REFERER]);
}

TEST(HttpRequestTest, any_number_of_spaces_between_colon_and_value)
{
    http::Request request;
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ":   " + host_data;

    std::string referer_data = "http://en.wikipedia.org/wiki/Main_Page";
    std::string referer_header = REFERER + ":           " + referer_data;

    request >> REQUEST_LINE >> CRLF >> host_header;
    EXPECT_EQ("", request[HOST]);

    request >> CRLF;
    EXPECT_EQ(host_data, request[HOST]);

    request >> referer_header;
    EXPECT_EQ("", request[REFERER]);

    request >> CRLF;
    EXPECT_EQ(referer_data, request[REFERER]);
}

TEST(HttpRequestTest,
     header_lines_starting_with_whitespace_are_part_of_the_previous_header_line)
{

    std::string referer_data_first_line =
        "http://en.wikipedia.org/wiki/Main_Page";
    std::string referer_data_second_line = "/wow/this/is/a/long/referer";

    {
        http::Request request;
        std::string separator = "\t";
        std::string referer_header = REFERER + ":" + referer_data_first_line +
                                     CRLF + separator +
                                     referer_data_second_line + CRLF;

        request >> REQUEST_LINE >> CRLF >> referer_header >> CRLF;
        EXPECT_EQ(referer_data_first_line + referer_data_second_line,
                  request[REFERER]);
    }
    {
        http::Request request;
        std::string separator = " ";
        std::string referer_header = REFERER + ":" + referer_data_first_line +
                                     CRLF + separator +
                                     referer_data_second_line + CRLF;

        request >> REQUEST_LINE >> CRLF >> referer_header >> CRLF;
        EXPECT_EQ(referer_data_first_line + referer_data_second_line,
                  request[REFERER]);
    }
    {
        http::Request request;
        std::string separator = "      ";
        std::string referer_header = REFERER + ":" + referer_data_first_line +
                                     CRLF + separator +
                                     referer_data_second_line + CRLF;

        request >> REQUEST_LINE >> CRLF >> referer_header >> CRLF;
        EXPECT_EQ(referer_data_first_line + referer_data_second_line,
                  request[REFERER]);
    }
}

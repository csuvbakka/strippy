#include <gtest/gtest.h>
#include <http_request.hpp>

#include <unordered_map>

namespace
{
const std::string HOST = "Host";
const std::string REFERER = "Referer";
const std::string CRLF = "\r\n";

const std::string REQUEST_LINE = "GET /path/to/file/index.html HTTP/1.0";

class RequestStringBuilder
{
public:
    friend RequestStringBuilder& operator>>(RequestStringBuilder& lhs,
                                            const std::string& rhs);

    RequestStringBuilder& request_line(const std::string& line)
    {
        request_line_ = line;
        return *this;
    }

    RequestStringBuilder& header(const std::string& header,
                                 const std::string& value)
    {
        headers_[header] = value;
        return *this;
    }

    std::string build()
    {
        std::string output = request_line_ + CRLF;
        for (const auto& kv : headers_)
            output += kv.first + ": " + kv.second + CRLF;

        output += CRLF;
        return output;
    }

    std::string data() { return request_string_; }
    std::string last() { return last_added_; }

    std::string get_request_line() { return request_line_; }
    std::string get_header(const std::string& header)
    {
        return headers_[header];
    }

private:
    std::string request_string_;
    std::string last_added_;
    std::string request_line_;
    std::unordered_map<std::string, std::string> headers_;
};

RequestStringBuilder& operator>>(RequestStringBuilder& lhs,
                                 const std::string& rhs)
{
    lhs.request_string_ += rhs;
    lhs.last_added_ = rhs;
    return lhs;
}
}

TEST(HttpRequestTest, first_line_is_the_request_line)
{
    http::Request request;
    RequestStringBuilder builder;
    builder.request_line(REQUEST_LINE);

    request >> builder.build();

    EXPECT_EQ(builder.get_request_line(), request.request_line());
}

TEST(HttpRequestTest, parse_request_line_in_multiple_chunks)
{
    http::Request request;
    RequestStringBuilder builder;

    builder >> "GET /path/to/fi";
    request >> builder.data();
    EXPECT_EQ("", request.request_line());

    builder >> "le/index.html HTTP/1.0";
    request >> builder.last();
    EXPECT_EQ("", request.request_line());

    request >> CRLF;
    EXPECT_EQ(builder.data(), request.request_line());
}

TEST(HttpRequestTest, ignore_empty_lines_before_request_line)
{
    http::Request request;
    RequestStringBuilder builder;
    builder.request_line(REQUEST_LINE);

    request >> CRLF + builder.build();

    EXPECT_EQ(builder.get_request_line(), request.request_line());
}

TEST(HttpRequestTest, parse_one_header)
{
    http::Request request;
    RequestStringBuilder builder;

    builder.request_line(REQUEST_LINE).header(HOST, "127.0.0.1");
    request >> builder.build();

    EXPECT_EQ(builder.get_request_line(), request.request_line());
    EXPECT_EQ(builder.get_header(HOST), request[HOST]);
}

TEST(HttpRequestTest, parse_one_header_in_chunks)
{
    http::Request request;
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ": " + host_data;
    request >> REQUEST_LINE >> CRLF >> host_header >> CRLF;

    EXPECT_EQ(host_data, request[HOST]);
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

TEST(HttpRequestTest, parse_two_headers_with_newline_in_between)
{
    http::Request request;
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ": " + host_data;

    std::string referer_data = "http://en.wikipedia.org/wiki/Main_Page";
    std::string referer_header = REFERER + ":" + referer_data;
    request >> REQUEST_LINE >> CRLF >> host_header.substr(0, 5) >>
        host_header.substr(5) + CRLF + referer_header.substr(0, 5) >>
        referer_header.substr(5) >> CRLF + CRLF;

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

TEST(HttpRequestTest, double_crlf_ends_message)
{
    http::Request request;
    RequestStringBuilder builder;

    builder.request_line(REQUEST_LINE)
        .header(HOST, "localhost")
        .header(REFERER, "google.com");

    request >> builder.build();

    EXPECT_EQ(builder.get_header(HOST), request[HOST]);
    EXPECT_EQ(builder.get_header(REFERER), request[REFERER]);
    EXPECT_TRUE(request.done_parsing());
}

TEST(HttpRequestTest, nothing_is_parsed_after_double_crlf)
{
    http::Request request;
    RequestStringBuilder builder;

    builder.request_line(REQUEST_LINE).header(HOST, "localhost");

    request >> builder.build();
    EXPECT_TRUE(request.done_parsing());

    builder.header(REFERER, "google.com");
    request >> builder.build();

    EXPECT_TRUE(request.done_parsing());
    EXPECT_EQ("", request[REFERER]);
}

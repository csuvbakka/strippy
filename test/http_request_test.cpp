#include <gtest/gtest.h>
#include <http_request.hpp>

#include <unordered_map>

namespace
{
const std::string HOST = "Host";
const std::string REFERER = "Referer";
const std::string CRLF = "\r\n";
const char* CRLFp = "\r\n";

const std::string REQUEST_LINE = "GET /path/to/file/index.html HTTP/1.0";

// const char* HOST = "Host";
// const char* REFERER = "Referer";
// const char* CRLF = "\r\n";

// const char* REQUEST_LINE = "GET /path/to/file/index.html HTTP/1.0";

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

    const char* data() { return request_string_.c_str(); }
    const char* last() { return last_added_.c_str(); }

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
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    RequestStringBuilder builder;
    builder.request_line(REQUEST_LINE);

    buffer += builder.build().c_str();
    request.process_buffer();

    EXPECT_EQ(builder.get_request_line(), request.request_line());
}

TEST(HttpRequestTest, parse_request_line_in_multiple_chunks)
{
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    RequestStringBuilder builder;

    builder >> "GET /path/to/fi";
    buffer += builder.data();
    request.process_buffer();
    EXPECT_EQ("", request.request_line());

    builder >> "le/index.html HTTP/1.0";
    buffer += builder.last();
    request.process_buffer();
    EXPECT_EQ("", request.request_line());

    buffer += CRLFp;
    request.process_buffer();
    EXPECT_EQ(builder.data(), request.request_line());
}

TEST(HttpRequestTest, ignore_empty_lines_before_request_line)
{
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    RequestStringBuilder builder;
    builder.request_line(REQUEST_LINE);

    buffer += CRLFp;
    request.process_buffer();
    buffer += builder.build().c_str();
    request.process_buffer();

    EXPECT_EQ(builder.get_request_line(), request.request_line());
}

TEST(HttpRequestTest, parse_one_header)
{
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    RequestStringBuilder builder;

    builder.request_line(REQUEST_LINE).header(HOST, "127.0.0.1");
    buffer += builder.build().c_str();
    request.process_buffer();

    EXPECT_EQ(builder.get_request_line(), request.request_line());
    EXPECT_EQ(builder.get_header(HOST), request[HOST]);
}

TEST(HttpRequestTest, parse_one_header_in_chunks)
{
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ": " + host_data;
    buffer += REQUEST_LINE.c_str();
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();
    buffer += host_header.c_str();
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();

    EXPECT_EQ(host_data, request[HOST]);
}

TEST(HttpRequestTest, parse_two_headers)
{
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ": " + host_data;

    std::string referer_data = "http://en.wikipedia.org/wiki/Main_Page";
    std::string referer_header = REFERER + ":" + referer_data;
    buffer += REQUEST_LINE.c_str();
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();
    buffer += host_header.c_str();
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();
    buffer += referer_header.c_str();
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();

    EXPECT_EQ(host_data, request[HOST]);
    EXPECT_EQ(referer_data, request[REFERER]);

    buffer.clear();
    http::Request request_full(buffer);
    buffer += (REQUEST_LINE + CRLF + host_header + CRLF + referer_header + CRLF)
                  .c_str();
    request_full.process_buffer();

    EXPECT_EQ(host_data, request_full[HOST]);
    EXPECT_EQ(referer_data, request_full[REFERER]);
}

TEST(HttpRequestTest, parse_two_headers_with_newline_in_between)
{
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ": " + host_data;

    std::string referer_data = "http://en.wikipedia.org/wiki/Main_Page";
    std::string referer_header = REFERER + ":" + referer_data;
    buffer += REQUEST_LINE.c_str();
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();
    buffer += host_header.substr(0, 5).c_str();
    request.process_buffer();
    buffer += host_header.substr(5).c_str();
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();
    buffer += referer_header.substr(0, 5).c_str();
    request.process_buffer();
    buffer += referer_header.substr(5).c_str();
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();

    EXPECT_EQ(host_data, request[HOST]);
    EXPECT_EQ(referer_data, request[REFERER]);

    buffer.clear();
    http::Request request_full(buffer);
    buffer += (REQUEST_LINE + CRLF + host_header + CRLF + referer_header + CRLF)
                  .c_str();
    request_full.process_buffer();

    EXPECT_EQ(host_data, request_full[HOST]);
    EXPECT_EQ(referer_data, request_full[REFERER]);
}

TEST(HttpRequestTest, parse_two_headers_small_chunks)
{
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ": " + host_data;

    std::string referer_data = "http://en.wikipedia.org/wiki/Main_Page";
    std::string referer_header = REFERER + ":" + referer_data;

    buffer += REQUEST_LINE.c_str();
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();
    buffer += host_header.c_str();
    request.process_buffer();
    EXPECT_EQ("", request[HOST]);

    buffer += CRLFp;
    request.process_buffer();
    EXPECT_EQ(host_data, request[HOST]);

    buffer += referer_header.c_str();
    request.process_buffer();
    EXPECT_EQ("", request[REFERER]);

    buffer += CRLFp;
    request.process_buffer();
    EXPECT_EQ(referer_data, request[REFERER]);
}

TEST(HttpRequestTest, any_number_of_spaces_between_colon_and_value)
{
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    std::string host_data = "127.0.0.1";
    std::string host_header = HOST + ":   " + host_data;

    std::string referer_data = "http://en.wikipedia.org/wiki/Main_Page";
    std::string referer_header = REFERER + ":           " + referer_data;

    buffer += REQUEST_LINE.c_str();
    request.process_buffer();
    buffer += CRLFp;
    request.process_buffer();
    buffer += host_header.c_str();
    request.process_buffer();
    EXPECT_EQ("", request[HOST]);

    buffer += CRLFp;
    request.process_buffer();
    EXPECT_EQ(host_data, request[HOST]);

    buffer += referer_header.c_str();
    request.process_buffer();
    EXPECT_EQ("", request[REFERER]);

    buffer += CRLFp;
    request.process_buffer();
    EXPECT_EQ(referer_data, request[REFERER]);
}

TEST(HttpRequestTest,
     header_lines_starting_with_whitespace_are_part_of_the_previous_header_line)
{
    mystr::MyStringBuffer buffer;

    std::string referer_data_first_line =
        "http://en.wikipedia.org/wiki/Main_Page";
    std::string referer_data_second_line = "/wow/this/is/a/long/referer";

    {
        http::Request request(buffer);
        std::string separator = "\t";
        std::string referer_header = REFERER + ":" + referer_data_first_line +
                                     CRLF + separator +
                                     referer_data_second_line + CRLF;

        buffer += REQUEST_LINE.c_str();
        request.process_buffer();
        buffer += CRLFp;
        request.process_buffer();
        buffer += referer_header.c_str();
        request.process_buffer();
        buffer += CRLFp;
        request.process_buffer();
        EXPECT_EQ(referer_data_first_line + referer_data_second_line,
                  request[REFERER]);
    }
    {
        http::Request request(buffer);
        std::string separator = " ";
        std::string referer_header = REFERER + ":" + referer_data_first_line +
                                     CRLF + separator +
                                     referer_data_second_line + CRLF;

        buffer += REQUEST_LINE.c_str();
        request.process_buffer();
        buffer += CRLFp;
        request.process_buffer();
        buffer += referer_header.c_str();
        request.process_buffer();
        buffer += CRLFp;
        request.process_buffer();
        EXPECT_EQ(referer_data_first_line + referer_data_second_line,
                  request[REFERER]);
    }
    {
        http::Request request(buffer);
        std::string separator = "      ";
        std::string referer_header = REFERER + ":" + referer_data_first_line +
                                     CRLF + separator +
                                     referer_data_second_line + CRLF;

        buffer += REQUEST_LINE.c_str();
        request.process_buffer();
        buffer += CRLFp;
        request.process_buffer();
        buffer += referer_header.c_str();
        request.process_buffer();
        buffer += CRLFp;
        request.process_buffer();
        EXPECT_EQ(referer_data_first_line + referer_data_second_line,
                  request[REFERER]);
    }
}

TEST(HttpRequestTest, double_crlf_ends_message)
{
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    RequestStringBuilder builder;

    builder.request_line(REQUEST_LINE)
        .header(HOST, "localhost")
        .header(REFERER, "google.com");

    buffer += builder.build().c_str();
    request.process_buffer();

    EXPECT_EQ(builder.get_header(HOST), request[HOST]);
    EXPECT_EQ(builder.get_header(REFERER), request[REFERER]);
    EXPECT_TRUE(request.done_parsing());
}

TEST(HttpRequestTest, nothing_is_parsed_after_double_crlf)
{
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    RequestStringBuilder builder;

    builder.request_line(REQUEST_LINE).header(HOST, "localhost");

    buffer += builder.build().c_str();
    request.process_buffer();
    EXPECT_TRUE(request.done_parsing());

    builder.header(REFERER, "google.com");
    buffer += builder.build().c_str();
    request.process_buffer();

    EXPECT_TRUE(request.done_parsing());
    EXPECT_EQ("", request[REFERER]);
}

TEST(HttpRequestTest, request_with_data)
{
    mystr::MyStringBuffer buffer;
    http::Request request(buffer);
    RequestStringBuilder builder;

    builder.request_line(REQUEST_LINE)
        .header(HOST, "localhost")
        .header(REFERER, "127.0.0.1")
        .header("Content-Length", "4");

    buffer += builder.build().c_str();
    request.process_buffer();

    buffer += "alma";
    request.process_buffer();

    EXPECT_TRUE(request.done_parsing());
    EXPECT_EQ("", request[REFERER]);
    EXPECT_EQ("alma", request.content());
}

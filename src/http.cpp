#include <http.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <tuple>

namespace
{
bool starts_with(const std::string& str, const std::string& prefix)
{
    return std::equal(prefix.begin(), prefix.end(), str.begin());
}

void split(const std::string& s, char delim, std::vector<std::string>& elems)
{
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim))
    {
        elems.push_back(item);
    }
}

std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::tuple<std::string, std::string> split_first(const std::string& s,
                                                 char delim)
{
    auto pos = s.find(delim);
    if (pos + 1 < s.length())
        return std::make_tuple(s.substr(0, pos), s.substr(pos + 1));
    else
        return std::make_tuple(s, std::string());
}

std::string ltrim(const std::string& str)
{
    if (str.empty())
        return str;

    return str.substr(str.find_first_not_of(' '));
}

std::string rtrim(const std::string& str)
{
    if (str.empty())
        return str;

    return str.substr(0, str.find_last_not_of(' ') + 1);
}

std::string trim(const std::string& str)
{
    return (rtrim(ltrim(str)));
}
}

HttpRequest::HttpRequest(const std::string& data)
{
    if (data.empty())
        return;

    auto lines = split(data, '\n');

    request_line_ = lines[0];
    if (starts_with(request_line_, "GET"))
        request_type_ = HttpRequestType::GET;
    else if (starts_with(request_line_, "POST"))
        request_type_ = HttpRequestType::POST;
    else
        request_type_ = HttpRequestType::OTHER;

    lines.erase(lines.begin());

    std::string header, header_data;
    for (const auto& line : lines)
    {
        std::tie(header, header_data) = split_first(line, ':');
        std::cout << header << ":" << header_data << std::endl;
        if (!header.empty())
            headers_[header] = trim(header_data);
    }
}

std::string HttpRequest::operator[](const std::string& header_string)
{
    auto it = headers_.find(header_string);
    if (it != headers_.end())
        return it->second;
    else
        return std::string();
}

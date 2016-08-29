#include <string_utils.hpp>

namespace util
{
namespace string
{
bool starts_with(const std::string& str, char c)
{
    return str.front() == c;
}
bool starts_with(const std::string& str, const std::string& prefix)
{
    return std::equal(prefix.begin(), prefix.end(), str.begin());
}

bool starts_with_whitespace(const std::string& str)
{
    return starts_with(str, ' ') || starts_with(str, '\t');
}

bool ends_with(const std::string& value, const std::string& ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void remove_leading(std::string& str, char prefix)
{
    if (starts_with(str, prefix))
        str.erase(0, 1);
}
void remove_leading(std::string& str, const std::string& prefix)
{
    if (starts_with(str, prefix))
        str.erase(0, prefix.length());
}

std::string join(const std::vector<std::string>& strings,
                 const std::string& delim)
{
    std::string joined;
    for (const auto& s : strings)
        joined += s + delim;

    return joined;
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

std::tuple<std::string, std::string> split_first(const std::string& s,
                                                 const std::string& delim)
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
}

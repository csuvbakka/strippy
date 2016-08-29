#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace http
{
class HeaderParser
{
public:
    friend HeaderParser& operator>>(HeaderParser& lhs, const std::string& rhs);

    std::unordered_map<std::string, std::string> headers();

private:
    std::vector<std::string> lines_;
};
HeaderParser& operator>>(HeaderParser& lhs, const std::string& rhs);
}

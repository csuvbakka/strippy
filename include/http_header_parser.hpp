#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "my_string.hpp"

namespace http
{
class HeaderParser
{
public:
    HeaderParser(const mystr::MyString& str);

private:
    std::unordered_map<std::string, std::string> headers_;
    mystr::MyString string_;
};
}

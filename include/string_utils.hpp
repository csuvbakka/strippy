#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <tuple>

namespace util
{
namespace string
{
bool starts_with(const std::string& str, const std::string& prefix);
bool ends_with(const std::string& value, const std::string& ending);

void split(const std::string& s, char delim, std::vector<std::string>& elems);
std::vector<std::string> split(const std::string& s, char delim);
std::tuple<std::string, std::string> split_first(const std::string& s,
                                                 char delim);
std::tuple<std::string, std::string> split_first(const std::string& s,
                                                 const std::string& delim);

std::string ltrim(const std::string& str);
std::string rtrim(const std::string& str);
std::string trim(const std::string& str);
}
}

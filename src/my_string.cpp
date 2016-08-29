#pragma once

#include <vector>

namespace mystr
{
template <typename T> class MyString
{
    MyString(const char* str);

private:
    std::vector<T> buffer_;
};
}

#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <memory>
#include <iterator>
#include <tuple>

#include <iostream>

namespace mystr
{
using buffer_type = std::array<char, 8192>;

class MyString
{
public:
    using iterator = typename buffer_type::iterator;
    using const_iterator = typename buffer_type::const_iterator;

    MyString(buffer_type& buffer, const char* str)
        : begin_(buffer.begin())
        , end_(buffer.begin())
    {
        auto str_length = std::strlen(str);
        std::memcpy(buffer.data(), str, str_length);
        std::advance(end_, str_length);
    }
    MyString(buffer_type& buffer, const std::string& str)
        : begin_(buffer.begin())
        , end_(buffer.begin())
    {
        std::copy(str.begin(), str.end(), buffer.data());
        std::advance(end_, str.length());
    }
    MyString(const MyString& other)
        : begin_(other.begin_)
        , end_(other.end_)
    {
    }
    MyString(MyString&& other)
        : begin_(std::move(other.begin_))
        , end_(std::move(other.end_))
    {
    }

    // TODO: make this operator+= and check bounds
    void append(const std::string& str)
    {
        for (const auto& c : str)
            *end_++ = c;
    }

    const_iterator cbegin() const { return begin_; }
    const_iterator cend() const { return end_; }

    std::string str() const { return std::string(begin_, end_); }

    const_iterator find(char c) const { return std::find(begin_, end_, c); }
    bool starts_with(const char* str)
    {
        auto begin = begin_;
        for (auto i = 0; i < std::strlen(str); ++i, std::advance(begin, 1))
        {
            if (str[i] != *begin)
                return false;
        }
        return true;
    }

private:
    iterator begin_;
    iterator end_;
};
}

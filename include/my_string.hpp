#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <exception>
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

    MyString(buffer_type& buffer)
        : buffer_(buffer)
        , begin_(buffer.begin())
        , end_(buffer.begin())
    {
    }
    MyString(const buffer_type& buffer, const_iterator begin,
             const_iterator end)
        : buffer_(buffer)
        , begin_(begin)
        , end_(end)
    {
    }
    MyString(const MyString& other)
        : buffer_(other.buffer_)
        , begin_(other.begin_)
        , end_(other.end_)
    {
    }
    MyString(MyString&& other)
        : buffer_(other.buffer_)
        , begin_(std::move(other.begin_))
        , end_(std::move(other.end_))
    {
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

    void pop_front()
    {
        if (end_ != begin_)
            std::advance(begin_, 1);
    }
    void pop_back()
    {
        if (end_ != begin_)
            std::advance(end_, -1);
    }

    std::size_t length() const { return std::distance(begin_, end_); }

    MyString substr(std::size_t pos)
    {
        if (length() < pos)
            throw std::out_of_range{"MyString substr"};
        else if (length() == pos)
            return MyString{buffer_, begin_, begin_};
        else
            return MyString{buffer_, std::next(begin_, pos), end_};
    }

private:
    const buffer_type& buffer_;
    const_iterator begin_;
    const_iterator end_;
};
}

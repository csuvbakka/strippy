#pragma once

#include <algorithm>
#include <cstring>
#include <vector>

#include <iostream>

namespace mystr
{
template <typename T> class MyString
{
    // friend class MySubString<T>;

public:
    using const_iterator = typename std::vector<T>::const_iterator;
    MyString(const char* str)
    {
        buffer_.reserve(strlen(str));
        for (auto i = 0; i < strlen(str); ++i)
            buffer_.push_back(str[i]);
    }

    const_iterator begin() const { return buffer_.begin(); }
    const_iterator end() const { return buffer_.end(); }

    std::string str() { return std::string(buffer_.begin(), buffer_.end()); }
    std::string str(const_iterator begin) const
    {
        return std::string(begin, buffer_.end());
    }

private:
    std::vector<T> buffer_;
};

template <typename C> class MySubString
{
    typedef typename MyString<C>::const_iterator MyString_const_iterator;

public:
    MySubString(const MyString<C>& mystr)
        : begin_(mystr.begin())
        , end_(mystr.end())
    {
    }
    MySubString(MyString_const_iterator begin, MyString_const_iterator end)
        : begin_(begin)
        , end_(end)
    {
    }

    std::string str() { return std::string(begin_, end_); }

private:
    MyString_const_iterator begin_;
    MyString_const_iterator end_;
};
}

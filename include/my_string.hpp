#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <exception>
#include <iterator>
#include <tuple>

#include <iostream>

namespace mystr
{
using buffer_type = std::array<char, 8192>;

class MyStringBuffer
{
public:
    MyStringBuffer()
        : begin_(buffer_.begin())
        , end_(buffer_.begin())
    {
    }
    using iterator = typename buffer_type::iterator;
    using const_iterator = typename buffer_type::const_iterator;

    const_iterator begin() const { return begin_; }
    const_iterator end() const { return end_; }

    void clear() { end_ = begin_; }
    std::size_t size() const { return std::distance(begin_, end_); }
    bool is_empty() const { return begin_ == end_; }

    bool operator==(const MyStringBuffer& rhs) const
    {
        return buffer_ == rhs.buffer_;
    }
    bool operator!=(const MyStringBuffer& rhs) const { return !(*this == rhs); }
    MyStringBuffer& operator+=(const std::string& str)
    {
        if (size() + str.length() > buffer_.max_size())
            throw std::out_of_range("MyStringBuffer operator+=");

        std::copy(std::begin(str), std::end(str), end_);
        std::advance(end_, str.length());
        return *this;
    }
    std::string str() const { return std::string(begin_, end_); }

    void erase_head(std::size_t len) { std::advance(begin_, len); }
    void erase_head(iterator it) { begin_ = it; }

    bool starts_with(const std::string& prefix) const
    {
        return std::equal(prefix.begin(), prefix.end(), begin_);
    }

    iterator find_first_of(char c) { return std::find(begin_, end_, c); }

private:
private:
    buffer_type buffer_;
    iterator begin_;
    iterator end_;
};

class MyString
{
public:
    using iterator = typename MyStringBuffer::iterator;
    using const_iterator = typename MyStringBuffer::const_iterator;

    MyString(MyStringBuffer& buffer)
        : buffer_(buffer)
        , begin_(buffer.begin())
        , end_(buffer.begin())
    {
    }
    MyString(const MyStringBuffer& buffer, const_iterator begin,
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

    bool operator==(const MyString& rhs)
    {
        return (begin_ == rhs.begin_ && end_ == rhs.end_);
    }
    MyString& operator=(const MyString& rhs)
    {
        if (*this == rhs)
            return *this;

        if (buffer_ != rhs.buffer_)
            throw std::logic_error{
                "MyString operator= exception: Buffers must be the same!"};

        begin_ = rhs.begin_;
        end_ = rhs.end_;

        return *this;
    }

    const_iterator begin() const { return begin_; }
    const_iterator end() const { return end_; }

    std::string str() const { return std::string(begin_, end_); }

    const_iterator find(char c) const { return std::find(begin_, end_, c); }
    const_iterator find_first_not(char c) const
    {
        return std::find_if(begin_, end_, [&c](char actual_char)
                            {
                                return actual_char != c;
                            });
    }
    const_iterator find_last_not(char c) const
    {
        if (is_empty())
            return end_;

        auto rit = std::find_if(std::make_reverse_iterator(end_),
                                std::make_reverse_iterator(begin_),
                                [&c](char actual_char)
                                {
                                    return actual_char != c;
                                });
        auto it = rit.base();
        std::advance(it, -1);
        return it;
    }
    bool starts_with(char c) const { return *begin_ == c; }
    bool starts_with(const char* str) const
    {
        auto begin = begin_;
        for (auto i = 0; i < std::strlen(str); ++i, std::advance(begin, 1))
            if (str[i] != *begin)
                return false;

        return true;
    }

    char front() { return *begin_; }
    char back() { return *end_; }

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
    bool is_empty() const { return begin_ == end_; }

    MyString substr(std::size_t pos) const
    {
        if (length() < pos)
            throw std::out_of_range{"MyString substr"};
        else if (length() == pos)
            return MyString{buffer_, begin_, begin_};
        else
            return MyString{buffer_, std::next(begin_, pos), end_};
    }
    MyString substr(const_iterator begin) const
    {
        return MyString(buffer_, begin, end_);
    }

    std::tuple<MyString, MyString> split_first(char c)
    {
        auto it = find(c);
        if (it == end_)
            return std::make_tuple(MyString(buffer_, begin_, end_),
                                   MyString(buffer_, end_, end_));
        else if (std::next(it) == end_)
            return std::make_tuple(MyString(buffer_, begin_, it),
                                   MyString(buffer_, end_, end_));
        else
            return std::make_tuple(MyString(buffer_, begin_, it),
                                   MyString(buffer_, std::next(it), end_));
    }

    void ltrim() { begin_ = find_first_not(' '); }
    void rtrim()
    {
        auto it = find_last_not(' ');
        if (it != end_)
            end_ = std::next(it);
    }

private:
    const MyStringBuffer& buffer_;
    const_iterator begin_;
    const_iterator end_;
};

MyString ltrim_copy(const MyString& str);
}

#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <exception>
#include <functional>
#include <iterator>
#include <tuple>

#include <iostream>

namespace str
{
using buffer_type = std::array<char, 8192>;

class character_array
{
public:
    character_array()
        : begin_(buffer_.begin())
        , end_(buffer_.begin())
    {
    }
    using iterator = typename buffer_type::iterator;
    using const_iterator = typename buffer_type::const_iterator;

    iterator begin() const { return begin_; }
    iterator end() const { return end_; }
    const_iterator cbegin() const { return begin_; }
    const_iterator cend() const { return end_; }

    std::size_t size() const { return std::distance(begin_, end_); }
    std::size_t length() const { return size(); }
    std::size_t max_size() const { return buffer_.max_size(); }
    std::size_t capacity() const { return max_size(); }

    void clear() { end_ = begin_ = std::begin(buffer_); }
    bool empty() const { return begin_ == end_; }

    char& operator[](size_t pos) { return buffer_[pos]; }
    char front() { return *begin_; }
    char back() { return *std::next(end_, -1); }

    std::string data() const
    {
        // return std::string(std::begin(buffer_), cend());
        return std::string(std::begin(buffer_), cbegin());
    }

    bool operator==(const character_array& rhs) const
    {
        return buffer_ == rhs.buffer_;
    }
    bool operator!=(const character_array& rhs) const
    {
        return !(*this == rhs);
    }

    void append(const char* str, std::size_t len)
    {
        if (len > 0)
        {
            if (size() + len > buffer_.max_size())
                throw std::out_of_range("character_array operator+=");

            std::memcpy(end_, str, len);
            end_ += len;

            std::cout << "size after append: " << size() << std::endl;
        }
    }
    void push_back(char c)
    {
        if (size() == max_size())
            throw std::out_of_range("character_array push_back");

        *end_++ = c;
    }
    void pop_back() { --end_; }

    character_array& operator+=(const char* str)
    {
        auto str_length = std::strlen(str);
        if (size() + str_length > buffer_.max_size())
            throw std::out_of_range("character_array operator+=");

        std::memcpy(end_, str, str_length);
        end_ += str_length;

        return *this;
    }
    std::string str() const { return std::string(begin_, end_); }

    void erase_head(std::size_t len) { begin_ += len; }
    void erase_head_until(iterator it) { begin_ = it; }

    bool starts_with(const std::string& prefix) const
    {
        return std::equal(prefix.begin(), prefix.end(), begin_);
    }

    iterator find(char c) { return std::find(begin_, end_, c); }

private:
private:
    buffer_type buffer_;
    iterator begin_;
    iterator end_;
};

std::ostream& operator<<(std::ostream& os, const character_array& str);

template <typename T> const T& empty_buffer()
{
    static T empty_string_buffer;
    return empty_string_buffer;
}

template <typename string_type> class string_view
{
public:
    using iterator = typename string_type::iterator;
    using const_iterator = typename string_type::const_iterator;

    string_view()
        : string_(empty_buffer<string_type>())
        , begin_(string_.begin())
        , end_(string_.end())
    {
    }

    string_view(const string_type& buffer)
        : string_(buffer)
        , begin_(buffer.begin())
        , end_(buffer.end())
    {
    }
    string_view(const string_type& buffer, const_iterator begin,
                const_iterator end)
        : string_(buffer)
        , begin_(begin)
        , end_(end)
    {
    }
    string_view(const string_view& other)
        : string_(other.string_)
        , begin_(other.begin_)
        , end_(other.end_)
    {
    }
    string_view(string_view&& other)
        : string_(other.string_)
        , begin_(std::move(other.begin_))
        , end_(std::move(other.end_))
    {
    }

    bool operator==(const string_view& rhs) const
    {
        return (begin_ == rhs.begin_ && end_ == rhs.end_);
    }
    string_view& operator=(const string_view& rhs)
    {
        if (*this == rhs)
            return *this;

        // if (string_ != rhs.string_)
        // throw std::logic_error{
        // "string_view operator= exception: Buffers must be the same!"};

        begin_ = rhs.begin_;
        end_ = rhs.end_;

        return *this;
    }
    string_view& operator=(string_view&& rhs)
    {
        // if (string_ != rhs.string_)
        // throw std::logic_error{
        // "string_view operator= exception: Buffers must be the same!"};

        begin_ = std::move(rhs.begin_);
        end_ = std::move(rhs.end_);

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
    const_iterator find_first_not_of(const std::string& str) const
    {
        return std::find_if(begin_, end_, [&str](char actual_char)
                            {
                                return std::none_of(
                                    str.begin(), str.end(),
                                    [&actual_char](char not_of_char)
                                    {
                                        return actual_char == not_of_char;
                                    });
                            });
    }
    const_iterator find_last_not(char c) const
    {
        if (empty())
            return end_;

        auto rit = std::find_if(std::make_reverse_iterator(end_),
                                std::make_reverse_iterator(begin_),
                                [&c](char actual_char)
                                {
                                    return actual_char != c;
                                });
        auto it = rit.base();
        return --it;
    }
    bool starts_with(char c) const { return *begin_ == c; }
    bool starts_with(const char* str) const
    {
        auto begin = begin_;
        for (auto i = 0; i < std::strlen(str); ++i, ++begin)
            if (str[i] != *begin)
                return false;

        return true;
    }

    char front() const { return *begin_; }
    char back() const { return *std::next(end_, -1); }

    void pop_front() { ++begin_; }
    void pop_back() { --end_; }

    std::size_t size() const { return std::distance(begin_, end_); }
    std::size_t length() const { return size(); }
    bool empty() const { return begin_ == end_; }

    string_view substr(std::size_t pos) const
    {
        if (length() < pos)
            throw std::out_of_range{"string_view substr"};
        else if (length() == pos)
            return string_view{string_, begin_, begin_};
        else
            return string_view{string_, std::next(begin_, pos), end_};
    }
    string_view substr(const_iterator begin) const
    {
        return string_view(string_, begin, end_);
    }

    std::tuple<string_view, string_view> split_first(char c)
    {
        auto it = find(c);
        if (it == end_)
            return std::make_tuple(string_view(string_, begin_, end_),
                                   string_view(string_, end_, end_));
        else if (std::next(it) == end_)
            return std::make_tuple(string_view(string_, begin_, it),
                                   string_view(string_, end_, end_));
        else
            return std::make_tuple(string_view(string_, begin_, it),
                                   string_view(string_, std::next(it), end_));
    }

    void erase_head(size_t len)
    {
        if (std::distance(begin_, end_) >= len)
            begin_ += len;
    }
    void erase_tail(size_t len)
    {
        if (std::distance(begin_, end_) >= len)
            end_ -= len;
    }

    void ltrim() { begin_ = find_first_not_of("\t "); }
    void rtrim()
    {
        auto it = find_last_not(' ');
        if (it != end_)
            end_ = std::next(it);
    }

    void trim()
    {
        ltrim();
        rtrim();
    }

private:
    const string_type& string_;
    const_iterator begin_;
    const_iterator end_;
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const string_view<T>& str)
{
    os << str.str();
    return os;
}

template <typename T> string_view<T> ltrim_copy(const string_view<T>& str)
{
    if (str.empty())
        return {str};

    return str.substr(str.find_first_not(' '));
}
template <typename T> string_view<T> rtrim_copy(const string_view<T>& str)
{
    if (str.empty())
        return {str};

    string_view<T> copy(str);
    copy.rtrim();
    return copy;
}
template <typename T> string_view<T> trim_copy(const string_view<T>& str)
{

    if (str.empty())
        return {str};

    string_view<T> copy(str);
    copy.trim();
    return copy;
}
}

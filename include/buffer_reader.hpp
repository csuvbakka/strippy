#pragma once

#include <my_string.hpp>
#include <string_utils.hpp>

namespace str
{
template <typename receiver> class BufferReader
{
    using string_type = typename receiver::string_type;

public:
    using string_view = str::string_view<string_type>;

    BufferReader(int socket_fd, string_type& string)
        : receiver_(socket_fd)
        , string_(string)
    {
    }

    string_view readline()
    {
        auto it = std::find(std::begin(string_), std::end(string_), '\n');
        while (it == std::end(string_))
        {
            receiver_.receive(string_);
            it = std::find(std::begin(string_), std::end(string_), '\n');
        }

        auto line = string_view(string_, std::begin(string_), it);
        std::cout << line << std::endl;
        strip_cr(line);

        str::erase_head_until(string_, std::next(it));
        return line;
    }

    ssize_t read_chunk() { return receiver_.receive(string_); }

private:
    void strip_cr(string_view& str)
    {
        if (!str.empty())
            if (str.back() == '\r')
                str.pop_back();
    }

private:
    receiver receiver_;
    string_type& string_;
};
}

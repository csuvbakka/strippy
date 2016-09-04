#pragma once

#include <my_string.hpp>
#include <string_utils.hpp>

namespace str
{
template <typename receiver> class BufferReader
{
    using buffer_type = typename receiver::buffer_type;

public:
    using string_view = str::string_view<buffer_type>;

    BufferReader(int socket_fd)
        : receiver_(socket_fd)
    {
    }

    string_view readline()
    {
        auto it = std::find(std::begin(buffer_), std::end(buffer_), '\n');
        while (it == std::end(buffer_))
        {
            receiver_.receive(buffer_);
            it = std::find(std::begin(buffer_), std::end(buffer_), '\n');
        }

        auto line = string_view(buffer_, std::begin(buffer_), it);
        strip_cr(line);

        str::erase_head_until(buffer_, std::next(it));
        return line;
    }

    string_view buffer() const { return string_view(buffer_); }

private:
    void strip_cr(string_view& str)
    {
        if (!str.empty())
            if (str.back() == '\r')
                str.pop_back();
    }

private:
    buffer_type buffer_;
    receiver receiver_;
};
}

#include <my_string.hpp>

namespace str
{

template <typename T>
std::ostream& operator<<(std::ostream& os, const string_view<T>& str)
{
    os << str;
    return os;
}

template <>
std::ostream& operator<<(std::ostream& os,
                         const string_view<character_array>& str)
{
    os << str.str();
    return os;
}
const character_array& empty_buffer()
{
    static character_array empty_string_buffer;
    return empty_string_buffer;
}
}

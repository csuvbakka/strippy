#include <my_string.hpp>

#include <iostream>

namespace mystr
{

const MyStringBuffer& empty_buffer()
{
    static MyStringBuffer empty_string_buffer;
    return empty_string_buffer;
}

MyString ltrim_copy(const MyString& str)
{
    if (str.is_empty())
        return MyString{str};

    return str.substr(str.find_first_not(' '));
}

MyString rtrim_copy(const MyString& str)
{
    if (str.is_empty())
        return MyString{str};

    MyString copy(str);
    copy.rtrim();
    return copy;
}
MyString trim_copy(const MyString& str)
{
    if (str.is_empty())
        return MyString{str};

    MyString copy(str);
    copy.trim();
    return copy;
}
}

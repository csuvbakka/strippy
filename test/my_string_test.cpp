#include <gtest/gtest.h>
#include <my_string.hpp>
#include <algorithm>

namespace
{
mystr::MyStringBuffer new_buffer(const std::string& str)
{
    mystr::MyStringBuffer output;
    output += str;
    return output;
}

mystr::MyString mystring_from_buffer(const mystr::MyStringBuffer& buffer)
{
    auto end_pos = std::find(buffer.begin(), buffer.end(), '\0');
    return mystr::MyString(buffer, buffer.begin(), end_pos);
}
}

TEST(MyStringBuffer, append)
{
    mystr::MyStringBuffer buffer;

    buffer += "alma";
    EXPECT_EQ("alma", buffer.str());

    buffer += "banan";
    EXPECT_EQ("almabanan", buffer.str());
}

TEST(MyStringBuffer, length)
{
    mystr::MyStringBuffer buffer;
    std::string str = "alma";

    buffer += str;
    EXPECT_EQ(str, buffer.str());
    EXPECT_EQ(str.length(), buffer.size());
}

TEST(MyStringBuffer, clear)
{
    mystr::MyStringBuffer buffer;

    buffer += "alma";
    EXPECT_EQ("alma", buffer.str());

    buffer.clear();
    EXPECT_TRUE(buffer.is_empty());
    EXPECT_EQ("", buffer.str());
}

TEST(MyString, constructor)
{
    auto buffer = new_buffer("alma");
    auto mystring = mystring_from_buffer(buffer);
    EXPECT_EQ("alma", mystring.str());
}

TEST(MyString, length)
{
    std::string str = "alma";
    auto buffer = new_buffer(str);
    auto mystring = mystring_from_buffer(buffer);
    EXPECT_EQ(str.length(), mystring.length());
}

TEST(MyString, find_character)
{
    auto haystack = new_buffer("Far far away in a galaxy");
    char needle = 'x';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find(needle);
    EXPECT_EQ(*it, needle);
}

TEST(MyString, string_copy)
{
    auto buffer = new_buffer("alma");

    auto mystring = mystring_from_buffer(buffer);
    mystr::MyString other(mystring);
    EXPECT_EQ(mystring.str(), other.str());
}

TEST(MyString, string_starts_with)
{
    auto buffer = new_buffer("alma");

    auto mystring = mystring_from_buffer(buffer);
    EXPECT_TRUE(mystring.starts_with("a"));
    EXPECT_TRUE(mystring.starts_with("al"));
    EXPECT_TRUE(mystring.starts_with("alm"));
    EXPECT_TRUE(mystring.starts_with("alma"));
    EXPECT_FALSE(mystring.starts_with("b"));
}

TEST(MyString, pop_front)
{
    auto buffer = new_buffer("alma");

    auto mystring = mystring_from_buffer(buffer);

    mystring.pop_front();
    EXPECT_EQ("lma", mystring.str());
    mystring.pop_front();
    EXPECT_EQ("ma", mystring.str());
    mystring.pop_front();
    EXPECT_EQ("a", mystring.str());
    mystring.pop_front();
    EXPECT_EQ("", mystring.str());
    mystring.pop_front();
    EXPECT_EQ("", mystring.str());
}

TEST(MyString, pop_back)
{
    auto buffer = new_buffer("alma");

    auto mystring = mystring_from_buffer(buffer);

    mystring.pop_back();
    EXPECT_EQ("alm", mystring.str());
    mystring.pop_back();
    EXPECT_EQ("al", mystring.str());
    mystring.pop_back();
    EXPECT_EQ("a", mystring.str());
    mystring.pop_back();
    EXPECT_EQ("", mystring.str());
    mystring.pop_back();
    EXPECT_EQ("", mystring.str());
}

TEST(MyString, substring_pos_only)
{
    auto buffer = new_buffer("LongString");

    auto mystring = mystring_from_buffer(buffer);
    auto substring = mystring.substr(4);

    EXPECT_EQ("String", substring.str());
}

TEST(MyString, substring_pos_only_pos_equals_end_returns_empty_string)
{
    std::string str = "LongString";
    auto buffer = new_buffer(str);

    auto mystring = mystring_from_buffer(buffer);
    auto substring = mystring.substr(str.length());

    EXPECT_EQ("", substring.str());
}

TEST(MyString, substring_pos_only_out_of_bounds_throws_std_out_of_range)
{
    auto buffer = new_buffer("LongString");

    auto mystring = mystring_from_buffer(buffer);
    EXPECT_THROW(mystring.substr(44), std::out_of_range);
}

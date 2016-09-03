#include <gtest/gtest.h>
#include <my_string.hpp>
#include <algorithm>

namespace
{
mystr::MyStringBuffer new_buffer(const std::string& str)
{
    mystr::MyStringBuffer output;
    output += str.c_str();
    return output;
}

mystr::MyString mystring_from_buffer(const mystr::MyStringBuffer& buffer)
{
    auto end_pos = std::find(buffer.begin(), buffer.end(), '\0');
    return mystr::MyString(buffer, buffer.begin(), end_pos);
}
}

TEST(MyStringBuffer, append_string)
{
    mystr::MyStringBuffer buffer;

    buffer += "alma";
    EXPECT_EQ("alma", buffer.str());

    buffer += "banan";
    EXPECT_EQ("almabanan", buffer.str());
}

TEST(MyStringBuffer, append_const_char_p)
{
    mystr::MyStringBuffer buffer;

    buffer += "alma";
    EXPECT_EQ("alma", buffer.str());

    buffer += "banan";
    EXPECT_EQ("almabanan", buffer.str());
}

TEST(MyStringBuffer, append_char_array)
{
    mystr::MyStringBuffer buffer;
    char to_append[200] = {};

    const char* str = "alma";
    const char* str2 = "banan";

    std::memcpy(to_append, str, std::strlen(str));
    buffer += to_append;
    EXPECT_EQ("alma", buffer.str());

    std::memcpy(to_append, str2, std::strlen(str2));
    buffer += to_append;
    EXPECT_EQ("almabanan", buffer.str());
}

// TEST(MyStringBuffer, length)
// {
// mystr::MyStringBuffer buffer;
// std::string str = "alma";

// buffer += str;
// EXPECT_EQ(str, buffer.str());
// EXPECT_EQ(str.length(), buffer.size());
// }

TEST(MyStringBuffer, clear)
{
    mystr::MyStringBuffer buffer;

    buffer += "alma";
    EXPECT_EQ("alma", buffer.str());

    buffer.clear();
    EXPECT_TRUE(buffer.is_empty());
    EXPECT_EQ("", buffer.str());
}

TEST(MyStringBuffer, starts_with)
{
    mystr::MyStringBuffer buffer;

    buffer += "alma";
    EXPECT_EQ("alma", buffer.str());

    EXPECT_TRUE(buffer.starts_with("a"));
    EXPECT_TRUE(buffer.starts_with("al"));
    EXPECT_TRUE(buffer.starts_with("alm"));
    EXPECT_TRUE(buffer.starts_with("alma"));

    EXPECT_FALSE(buffer.starts_with("banan"));
}

TEST(MyStringBuffer, find_first_of)
{
    mystr::MyStringBuffer buffer;

    buffer += "alma";

    auto it = buffer.find_first_of('m');
    EXPECT_EQ(*it, 'm');
}

TEST(MyStringBuffer, find_first_of_not_found_return_end_iterator)
{
    mystr::MyStringBuffer buffer;

    buffer += "alma";

    auto it = buffer.find_first_of('x');
    EXPECT_EQ(it, buffer.end());
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

TEST(MyString, find_first_not)
{
    auto haystack = new_buffer("aaaxaaxaaax");
    char needle = 'a';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_first_not(needle);
    EXPECT_EQ(*it, 'x');
    EXPECT_EQ(3, std::distance(mystring.begin(), it));
}

TEST(MyString, find_first_not_needle_not_in_haystack_finds_begin)
{
    auto haystack = new_buffer("aaaxaaxaaax");
    char needle = 'c';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_first_not(needle);
    EXPECT_EQ(it, mystring.begin());
}

TEST(MyString, find_first_not_empty_string_finds_end)
{
    auto haystack = new_buffer("");
    char needle = 'c';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_first_not(needle);
    EXPECT_EQ(it, mystring.end());
}

TEST(MyString, find_first_not_of)
{
    auto haystack = new_buffer("abcbccbaXabcab");
    std::string not_of = "abc";

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_first_not_of(not_of);
    EXPECT_EQ(*it, 'X');
}

TEST(MyString, find_first_not_of_all_of_returns_end)
{
    auto haystack = new_buffer("abcbccbaabcab");
    std::string not_of = "abc";

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_first_not_of(not_of);
    EXPECT_EQ(it, mystring.end());
}

TEST(MyString, find_last_not)
{
    auto haystack = new_buffer("aaaxaaxaaa");
    char needle = 'a';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_last_not(needle);
    EXPECT_EQ(*it, 'x');
    EXPECT_EQ(4, std::distance(it, mystring.end()));
    EXPECT_EQ(6, std::distance(mystring.begin(), it));
}

TEST(MyString, find_last_not_empty_string_finds_end)
{
    auto haystack = new_buffer("");
    char needle = 'a';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_last_not(needle);
    EXPECT_EQ(it, mystring.end());
}

TEST(MyString, find_last_not_needle_is_first_character_finds_begin)
{
    auto haystack = new_buffer("axxxxxxxxx");
    char needle = 'x';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_last_not(needle);
    EXPECT_EQ(it, mystring.begin());
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

TEST(MyString, split_first)
{
    auto buffer = new_buffer("Long:String");

    auto mystring = mystring_from_buffer(buffer);
    mystr::MyString first{buffer}, second{buffer};
    std::tie(first, second) = mystring.split_first(':');
    EXPECT_EQ("Long", first.str());
    EXPECT_EQ("String", second.str());
}

TEST(MyString, split_first_separator_is_first_character)
{
    auto buffer = new_buffer(":LongString");

    auto mystring = mystring_from_buffer(buffer);
    mystr::MyString first{buffer}, second{buffer};
    std::tie(first, second) = mystring.split_first(':');
    EXPECT_EQ("", first.str());
    EXPECT_EQ("LongString", second.str());
}

TEST(MyString, split_first_separator_is_last_character)
{
    auto buffer = new_buffer("LongString:");

    auto mystring = mystring_from_buffer(buffer);
    mystr::MyString first{buffer}, second{buffer};
    std::tie(first, second) = mystring.split_first(':');
    EXPECT_EQ("LongString", first.str());
    EXPECT_EQ("", second.str());
}

TEST(MyString, ltrim)
{
    auto buffer = new_buffer("    alma");

    auto mystring = mystring_from_buffer(buffer);
    mystring.ltrim();
    EXPECT_EQ("alma", mystring.str());
}

TEST(MyString, ltrim_empty)
{
    auto buffer = new_buffer("");

    auto mystring = mystring_from_buffer(buffer);
    mystring.ltrim();
    EXPECT_EQ("", mystring.str());
}

TEST(MyString, ltrim_trims_only_left_side)
{
    auto buffer = new_buffer("   alma  ");

    auto mystring = mystring_from_buffer(buffer);
    mystring.ltrim();
    EXPECT_EQ("alma  ", mystring.str());
}

TEST(MyString, rtrim)
{
    auto buffer = new_buffer("alma    ");

    auto mystring = mystring_from_buffer(buffer);
    mystring.rtrim();
    EXPECT_EQ("alma", mystring.str());
}

TEST(MyString, rtrim_empty)
{
    auto buffer = new_buffer("");

    auto mystring = mystring_from_buffer(buffer);
    mystring.rtrim();
    EXPECT_EQ("", mystring.str());
}

TEST(MyString, rtrim_one_space)
{
    auto buffer = new_buffer("alma ");

    auto mystring = mystring_from_buffer(buffer);
    mystring.rtrim();
    EXPECT_EQ("alma", mystring.str());
}

TEST(MyString, rtrim_trims_only_right_side)
{
    auto buffer = new_buffer("  alma     ");

    auto mystring = mystring_from_buffer(buffer);
    mystring.rtrim();
    EXPECT_EQ("  alma", mystring.str());
}

TEST(MyString, trim)
{
    auto buffer = new_buffer("   alma    ");

    auto mystring = mystring_from_buffer(buffer);
    mystring.trim();
    EXPECT_EQ("alma", mystring.str());
}

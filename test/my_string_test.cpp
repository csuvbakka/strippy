#include <gtest/gtest.h>
#include <my_string.hpp>
#include <algorithm>
#include <string>

using char_array_view = str::string_view<str::character_array>;
using std_string_view = str::string_view<std::string>;

namespace
{
str::character_array new_buffer(const std::string& str)
{
    str::character_array output;
    output += str.c_str();
    return output;
}

char_array_view mystring_from_buffer(const str::character_array& buffer)
{
    auto end_pos = std::find(buffer.begin(), buffer.end(), '\0');
    return char_array_view(buffer, buffer.begin(), end_pos);
}
}

TEST(character_array, append_string)
{
    str::character_array buffer;

    buffer += "alma";
    EXPECT_EQ("alma", buffer.str());

    buffer += "banan";
    EXPECT_EQ("almabanan", buffer.str());
}

TEST(character_array, append_const_char_p)
{
    str::character_array buffer;

    buffer += "alma";
    EXPECT_EQ("alma", buffer.str());

    buffer += "banan";
    EXPECT_EQ("almabanan", buffer.str());
}

TEST(character_array, append_char_array)
{
    str::character_array buffer;
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

TEST(character_array, push_back)
{
    str::character_array buffer;

    buffer += "alma";
    EXPECT_EQ("alma", buffer.str());

    buffer.pop_back();
    EXPECT_EQ("alm", buffer.str());
    buffer.pop_back();
    EXPECT_EQ("al", buffer.str());
    buffer.pop_back();
    EXPECT_EQ("a", buffer.str());
    buffer.pop_back();
    EXPECT_TRUE(buffer.empty());
}

TEST(character_array, pop_back)
{
    str::character_array buffer;

    buffer.push_back('a');
    EXPECT_EQ("a", buffer.str());
    buffer.push_back('l');
    EXPECT_EQ("al", buffer.str());
    buffer.push_back('m');
    EXPECT_EQ("alm", buffer.str());
    buffer.push_back('a');
    EXPECT_EQ("alma", buffer.str());
}

TEST(character_array, data)
{
    str::character_array buffer;
    std::string str = "alma";

    buffer += str.c_str();
    EXPECT_EQ(str, buffer.data());
}

TEST(character_array, operator_square_brackets)
{
    str::character_array buffer;
    std::string str = "alma";

    buffer += str.c_str();
    for (auto i = 0; i < str.length(); ++i)
        EXPECT_EQ(str[i], buffer[i]);
}

TEST(character_array, front)
{
    str::character_array buffer;
    std::string str = "alma";

    buffer += str.c_str();
    EXPECT_EQ(str.front(), buffer.front());
}

TEST(character_array, back)
{
    str::character_array buffer;
    std::string str = "alma";

    buffer += str.c_str();
    EXPECT_EQ(str.back(), buffer.back());
}

TEST(character_array, length)
{
    str::character_array buffer;
    std::string str = "alma";

    buffer += str.c_str();
    EXPECT_EQ(str, buffer.str());
    EXPECT_EQ(str.length(), buffer.size());
}

TEST(character_array, clear)
{
    str::character_array buffer;

    buffer += "alma";
    EXPECT_EQ("alma", buffer.str());

    buffer.clear();
    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ("", buffer.str());
}

TEST(character_array, starts_with)
{
    str::character_array buffer;

    buffer += "alma";
    EXPECT_EQ("alma", buffer.str());

    EXPECT_TRUE(buffer.starts_with("a"));
    EXPECT_TRUE(buffer.starts_with("al"));
    EXPECT_TRUE(buffer.starts_with("alm"));
    EXPECT_TRUE(buffer.starts_with("alma"));

    EXPECT_FALSE(buffer.starts_with("banan"));
}

TEST(character_array, find)
{
    str::character_array buffer;

    buffer += "alma";

    auto it = buffer.find('m');
    EXPECT_EQ(*it, 'm');
}

TEST(character_array, find_first_of_not_found_return_end_iterator)
{
    str::character_array buffer;

    buffer += "alma";

    auto it = buffer.find('x');
    EXPECT_EQ(it, buffer.end());
}

TEST(string_view, constructor)
{
    auto buffer = new_buffer("alma");
    auto mystring = mystring_from_buffer(buffer);
    EXPECT_EQ("alma", mystring.str());
}

TEST(string_view, length)
{
    std::string str = "alma";
    auto buffer = new_buffer(str);
    auto mystring = mystring_from_buffer(buffer);
    EXPECT_EQ(str.length(), mystring.length());
}

TEST(string_view, find_character)
{
    auto haystack = new_buffer("Far far away in a galaxy");
    char needle = 'x';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find(needle);
    EXPECT_EQ(*it, needle);
}

TEST(string_view, find_first_not)
{
    auto haystack = new_buffer("aaaxaaxaaax");
    char needle = 'a';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_first_not(needle);
    EXPECT_EQ(*it, 'x');
    EXPECT_EQ(3, std::distance(mystring.begin(), it));
}

TEST(string_view, find_first_not_needle_not_in_haystack_finds_begin)
{
    auto haystack = new_buffer("aaaxaaxaaax");
    char needle = 'c';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_first_not(needle);
    EXPECT_EQ(it, mystring.begin());
}

TEST(string_view, find_first_not_empty_string_finds_end)
{
    auto haystack = new_buffer("");
    char needle = 'c';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_first_not(needle);
    EXPECT_EQ(it, mystring.end());
}

TEST(string_view, find_first_not_of)
{
    auto haystack = new_buffer("abcbccbaXabcab");
    std::string not_of = "abc";

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_first_not_of(not_of);
    EXPECT_EQ(*it, 'X');
}

TEST(string_view, find_first_not_of_all_of_returns_end)
{
    auto haystack = new_buffer("abcbccbaabcab");
    std::string not_of = "abc";

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_first_not_of(not_of);
    EXPECT_EQ(it, mystring.end());
}

TEST(string_view, find_last_not)
{
    auto haystack = new_buffer("aaaxaaxaaa");
    char needle = 'a';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_last_not(needle);
    EXPECT_EQ(*it, 'x');
    EXPECT_EQ(4, std::distance(it, mystring.end()));
    EXPECT_EQ(6, std::distance(mystring.begin(), it));
}

TEST(string_view, find_last_not_empty_string_finds_end)
{
    auto haystack = new_buffer("");
    char needle = 'a';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_last_not(needle);
    EXPECT_EQ(it, mystring.end());
}

TEST(string_view, find_last_not_needle_is_first_character_finds_begin)
{
    auto haystack = new_buffer("axxxxxxxxx");
    char needle = 'x';

    auto mystring = mystring_from_buffer(haystack);
    auto it = mystring.find_last_not(needle);
    EXPECT_EQ(it, mystring.begin());
}

TEST(string_view, string_copy)
{
    auto buffer = new_buffer("alma");

    auto mystring = mystring_from_buffer(buffer);
    char_array_view other(mystring);
    EXPECT_EQ(mystring.str(), other.str());
}

TEST(string_view, string_starts_with)
{
    auto buffer = new_buffer("alma");

    auto mystring = mystring_from_buffer(buffer);
    EXPECT_TRUE(mystring.starts_with("a"));
    EXPECT_TRUE(mystring.starts_with("al"));
    EXPECT_TRUE(mystring.starts_with("alm"));
    EXPECT_TRUE(mystring.starts_with("alma"));
    EXPECT_FALSE(mystring.starts_with("b"));
}

TEST(string_view, pop_front)
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
    EXPECT_TRUE(mystring.empty());
}

TEST(string_view, pop_back)
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
    EXPECT_TRUE(mystring.empty());
}

TEST(string_view, substring_pos_only)
{
    auto buffer = new_buffer("LongString");

    auto mystring = mystring_from_buffer(buffer);
    auto substring = mystring.substr(4);

    EXPECT_EQ("String", substring.str());
}

TEST(string_view, substring_pos_only_pos_equals_end_returns_empty_string)
{
    std::string str = "LongString";
    auto buffer = new_buffer(str);

    auto mystring = mystring_from_buffer(buffer);
    auto substring = mystring.substr(str.length());

    EXPECT_EQ("", substring.str());
}

TEST(string_view, substring_pos_only_out_of_bounds_throws_std_out_of_range)
{
    auto buffer = new_buffer("LongString");

    auto mystring = mystring_from_buffer(buffer);
    EXPECT_THROW(mystring.substr(44), std::out_of_range);
}

TEST(string_view, split_first)
{
    auto buffer = new_buffer("Long:String");

    auto mystring = mystring_from_buffer(buffer);
    char_array_view first{buffer}, second{buffer};
    std::tie(first, second) = mystring.split_first(':');
    EXPECT_EQ("Long", first.str());
    EXPECT_EQ("String", second.str());
}

TEST(string_view, split_first_separator_is_first_character)
{
    auto buffer = new_buffer(":LongString");

    auto mystring = mystring_from_buffer(buffer);
    char_array_view first{buffer}, second{buffer};
    std::tie(first, second) = mystring.split_first(':');
    EXPECT_EQ("", first.str());
    EXPECT_EQ("LongString", second.str());
}

TEST(string_view, split_first_separator_is_last_character)
{
    auto buffer = new_buffer("LongString:");

    auto mystring = mystring_from_buffer(buffer);
    char_array_view first{buffer}, second{buffer};
    std::tie(first, second) = mystring.split_first(':');
    EXPECT_EQ("LongString", first.str());
    EXPECT_EQ("", second.str());
}

TEST(string_view, ltrim)
{
    auto buffer = new_buffer("    alma");

    auto mystring = mystring_from_buffer(buffer);
    mystring.ltrim();
    EXPECT_EQ("alma", mystring.str());
}

TEST(string_view, ltrim_empty)
{
    auto buffer = new_buffer("");

    auto mystring = mystring_from_buffer(buffer);
    mystring.ltrim();
    EXPECT_EQ("", mystring.str());
}

TEST(string_view, ltrim_trims_only_left_side)
{
    auto buffer = new_buffer("   alma  ");

    auto mystring = mystring_from_buffer(buffer);
    mystring.ltrim();
    EXPECT_EQ("alma  ", mystring.str());
}

TEST(string_view, rtrim)
{
    auto buffer = new_buffer("alma    ");

    auto mystring = mystring_from_buffer(buffer);
    mystring.rtrim();
    EXPECT_EQ("alma", mystring.str());
}

TEST(string_view, rtrim_empty)
{
    auto buffer = new_buffer("");

    auto mystring = mystring_from_buffer(buffer);
    mystring.rtrim();
    EXPECT_EQ("", mystring.str());
}

TEST(string_view, rtrim_one_space)
{
    auto buffer = new_buffer("alma ");

    auto mystring = mystring_from_buffer(buffer);
    mystring.rtrim();
    EXPECT_EQ("alma", mystring.str());
}

TEST(string_view, rtrim_trims_only_right_side)
{
    auto buffer = new_buffer("  alma     ");

    auto mystring = mystring_from_buffer(buffer);
    mystring.rtrim();
    EXPECT_EQ("  alma", mystring.str());
}

TEST(string_view, trim)
{
    auto buffer = new_buffer("   alma    ");

    auto mystring = mystring_from_buffer(buffer);
    mystring.trim();
    EXPECT_EQ("alma", mystring.str());
}

TEST(string_view, constructor_std_string)
{
    std::string alma = "alma";
    std_string_view mystring(alma);

    EXPECT_EQ(alma, mystring.str());
}

TEST(string_view, trim_std_string)
{
    std::string alma = "   alma  ";
    std_string_view mystring(alma);

    mystring.trim();
    EXPECT_EQ("alma", mystring.str());
}

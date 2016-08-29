#include <gtest/gtest.h>
#include <my_string.hpp>

TEST(MyString, constructor_const_char_pointer)
{
    mystr::buffer_type buffer;
    mystr::MyString mystring(buffer, "alma");
    EXPECT_EQ("alma", mystring.str());
}

TEST(MyString, constructor_std_string)
{
    mystr::buffer_type buffer;
    std::string alma = "alma";
    mystr::MyString mystring(buffer, alma);
    EXPECT_EQ(alma, mystring.str());
}

TEST(MyString, find_character)
{
    mystr::buffer_type buffer;
    std::string haystack = "Far far away in a galaxy";
    char needle = 'x';

    mystr::MyString mystring(buffer, haystack);
    auto it = mystring.find(needle);
    EXPECT_EQ(*it, needle);
}

TEST(MyString, append_string)
{
    mystr::buffer_type buffer;
    std::string str = "str";
    std::string ing = "ing";

    mystr::MyString mystring(buffer, str);
    mystring.append(ing);
    EXPECT_EQ(str + ing, mystring.str());
}

TEST(MyString, string_copy)
{
    mystr::buffer_type buffer;
    std::string str = "alma";

    mystr::MyString mystring(buffer, str);
    mystr::MyString other(mystring);
    EXPECT_EQ(str, other.str());
}

TEST(MyString, string_starts_with)
{
    mystr::buffer_type buffer;
    std::string str = "alma";

    mystr::MyString mystring(buffer, str);
    EXPECT_TRUE(mystring.starts_with("a"));
    EXPECT_TRUE(mystring.starts_with("al"));
    EXPECT_TRUE(mystring.starts_with("alm"));
    EXPECT_TRUE(mystring.starts_with("alma"));
    EXPECT_FALSE(mystring.starts_with("b"));
}

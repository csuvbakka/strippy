#include <gtest/gtest.h>
#include <my_string.hpp>

TEST(MyString, constructor_const_char_pointer)
{
    mystr::MyString<char> mystring("alma");
    EXPECT_EQ("alma", mystring.str());
}

TEST(MySubString, constructor_const_char_pointer)
{
    mystr::MyString<char> mystring("alma");
    mystr::MySubString<char> substring(mystring);

    EXPECT_EQ("alma", substring.str());
}

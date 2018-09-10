#include <cstring>
#include <gtest/gtest.h>
#include <problem/string_problem.h>

TEST(string_problemTest, reverse) {
  {
    char input[] = "input";
    prob::reverse(input);
    ASSERT_EQ(0, std::strcmp("tupni", input));
  }
  {
    char input[] = "racecar";
    prob::reverse(input);
    ASSERT_EQ(0, std::strcmp("racecar", input));
  }
  {
    char input[] = "";
    prob::reverse(input);
    ASSERT_EQ(0, std::strcmp("", input));
  }
  {
    char input[] = "1";
    prob::reverse(input);
    ASSERT_EQ(0, std::strcmp("1", input));
  }
  {
    char input[] = "spooky";
    prob::reverse(input);
    ASSERT_EQ(0, std::strcmp("ykoops", input));
  }
}

TEST(string_problemTest, rec_reverse) {
  {
    char input[] = "input";
    prob::rec::reverse(input);
    ASSERT_EQ(0, std::strcmp("tupni", input));
  }
  {
    char input[] = "racecar";
    prob::rec::reverse(input);
    ASSERT_EQ(0, std::strcmp("racecar", input));
  }
  {
    char input[] = "";
    prob::rec::reverse(input);
    ASSERT_EQ(0, std::strcmp("", input));
  }
  {
    char input[] = "1";
    prob::rec::reverse(input);
    ASSERT_EQ(0, std::strcmp("1", input));
  }
  {
    char input[] = "spooky";
    prob::rec::reverse(input);
    ASSERT_EQ(0, std::strcmp("ykoops", input));
  }
}

TEST(string_problemTest, is_palindrome) {
  ASSERT_TRUE(prob::is_palindrome("racecar"));
  ASSERT_TRUE(prob::is_palindrome(""));
  ASSERT_TRUE(prob::is_palindrome("1"));
  ASSERT_FALSE(prob::is_palindrome("test"));
}

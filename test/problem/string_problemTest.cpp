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

TEST(string_problemTest, longest_palindromic_substring) {
  {
    const char *msg = "spooky";
    auto r = prob::rec::longest_palindromic_substring(msg);
    ASSERT_EQ(r, "oo");
    // printf("|%.*s|\n", r.length, r.str);
  }
  {
    const char *msg = "anna";
    auto r = prob::rec::longest_palindromic_substring(msg);
    ASSERT_EQ(r, "anna");
    // printf("|%.*s|\n", r.length, r.str);
  }
  {
    const char *msg = "racecar";
    auto r = prob::rec::longest_palindromic_substring(msg);
    ASSERT_EQ(r, "racecar");
    // printf("|%.*s|\n", r.length, r.str);
  }
  {
    const char *msg = "compilation";
    auto r = prob::rec::longest_palindromic_substring(msg);
    ASSERT_EQ(r, "c");
    // printf("|%.*s|\n", r.length, r.str);
  }
  {
    const char *msg = "database";
    auto r = prob::rec::longest_palindromic_substring(msg);
    ASSERT_EQ(r, "ata");
    // printf("|%.*s|\n", r.length, r.str);
  }
  {
    const char *msg = "loaded";
    auto r = prob::rec::longest_palindromic_substring(msg);
    ASSERT_EQ(r, "ded");
    // printf("|%.*s|\n", r.length, r.str);
  }
  // {
  //   const char *msg = "the quick annabrown fox racecarjumped over the lazy dog";
  //   auto r = prob::rec::longest_palindromic_substring(msg);
  //   ASSERT_EQ(r, "racecar");
  //   // printf("|%.*s|\n", r.length, r.str);
  // }
  {
    const char *msg = "forgeeksskeegfor";
    auto r = prob::rec::longest_palindromic_substring(msg);
    ASSERT_EQ(r, "geeksskeeg");
    // printf("|%.*s|\n", r.length, r.str);
  }
}

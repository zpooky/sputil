#include <gtest/gtest.h>
#include <np/longest_palindromic_substring.h>

TEST(longest_palindromic_substringTest, test) {
  {
    const char *msg = "abcde";
    auto r = np::rec::longest_palindromic_substring(msg);
    // ASSERT_EQ(r, "oo");
    // printf("|%.*s|\n", r.length, r.str);
  }
  // {
  //   const char *msg = "the quick annabrown fox racecarjumped over the lazy
  //   dog";
  //   auto r = prob::rec::longest_palindromic_substring(msg);
  //   ASSERT_EQ(r, "racecar");
  //   // printf("|%.*s|\n", r.length, r.str);
  // }
  // {
  //   const char *msg = "spooky";
  //   auto r = prob::rec::longest_palindromic_substring(msg);
  //   ASSERT_EQ(r, "oo");
  //   // printf("|%.*s|\n", r.length, r.str);
  // }
  // {
  //   const char *msg = "anna";
  //   auto r = prob::rec::longest_palindromic_substring(msg);
  //   ASSERT_EQ(r, "anna");
  //   // printf("|%.*s|\n", r.length, r.str);
  // }
  // {
  //   const char *msg = "racecar";
  //   auto r = prob::rec::longest_palindromic_substring(msg);
  //   ASSERT_EQ(r, "racecar");
  //   // printf("|%.*s|\n", r.length, r.str);
  // }
  // {
  //   const char *msg = "compilation";
  //   auto r = prob::rec::longest_palindromic_substring(msg);
  //   ASSERT_EQ(r, "c");
  //   // printf("|%.*s|\n", r.length, r.str);
  // }
  // {
  //   const char *msg = "database";
  //   auto r = prob::rec::longest_palindromic_substring(msg);
  //   ASSERT_EQ(r, "ata");
  //   // printf("|%.*s|\n", r.length, r.str);
  // }
  // {
  //   const char *msg = "loaded";
  //   auto r = prob::rec::longest_palindromic_substring(msg);
  //   ASSERT_EQ(r, "ded");
  //   // printf("|%.*s|\n", r.length, r.str);
  // }
  // {
  //   const char *msg = "forgeeksskeegfor";
  //   auto r = prob::rec::longest_palindromic_substring(msg);
  //   ASSERT_EQ(r, "geeksskeeg");
  //   // printf("|%.*s|\n", r.length, r.str);
  // }
}

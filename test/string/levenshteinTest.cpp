#include <gtest/gtest.h>
#include <string/levenshtein.h>

TEST(levenshteinTest, test) {
  ASSERT_EQ(0, ascii::levenshtein("test", "test"));
  ASSERT_EQ(1, ascii::levenshtein("tent", "test"));
  ASSERT_EQ(3, ascii::levenshtein("kitten", "sitting"));
  ASSERT_EQ(4, ascii::levenshtein("", "four"));
  ASSERT_EQ(4, ascii::levenshtein("four", ""));
}

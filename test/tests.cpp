#include "gtest/gtest.h"
#include <cstdio>

class ThrowListener : public testing::EmptyTestEventListener {
public:
  void
  OnTestPartResult(const testing::TestPartResult &result) override {
    if (result.type() == testing::TestPartResult::kFatalFailure) {
      throw testing::AssertionException(result);
    }
  }
};

int
main(int argc, char **argv) {
  printf("Here we go!\n");
  testing::InitGoogleTest(&argc, argv);

  testing::UnitTest::GetInstance()->listeners().Append(new ThrowListener);
  return RUN_ALL_TESTS();
}


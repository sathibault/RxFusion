#include "gtest/gtest.h"

#define CONSTRUCTED runAll();
void fatal(const char *msg) { puts(msg); exit(1); }
unsigned int epoch_timestamp() { return time(NULL); }

#include "rxfusion/rxfusion.h"
#include "test_scheduler.h"

TEST(OperatorTest, Of) {
  Doc<int> out;
  Range<int> r(1,10);

  r >> SumOf<int,3>() >> out;
  EXPECT_STREQ(out.text(), "6 15 24");
  out.clear();

  r >> AverageOf<int,3>() >> out;
  EXPECT_STREQ(out.text(), "2 5 8");
  out.clear();

  r >> MinOf<int,3>() >> out;
  EXPECT_STREQ(out.text(), "1 4 7");
  out.clear();

  r >> MaxOf<int,3>() >> out;
  EXPECT_STREQ(out.text(), "3 6 9");
}

TEST(OperatorTest, Over) {
  Doc<int> out;
  Range<int> r(1,9);

  r >> Iterate<int>(150) >> SumOver<int>(500) >> out;
  EXPECT_STREQ(out.text(), "6 15 24");
  out.clear();

  r >> Iterate<int>(150) >> AverageOver<int>(500) >> out;
  EXPECT_STREQ(out.text(), "2 5 8");
  out.clear();

  r >> Iterate<int>(150) >> MinOver<int>(500) >> out;
  EXPECT_STREQ(out.text(), "1 4 7");
  out.clear();

  r >> Iterate<int>(150) >> MaxOver<int>(500) >> out;
  EXPECT_STREQ(out.text(), "3 6 9");
}

TEST(OperatorTest, Misc) {
  Doc<int> out;
  Range<int> r(1,10);

  r >> Map<int,int>([](int x)->int { return x/3; }) >> Dedup<int>() >> out;
  EXPECT_STREQ(out.text(), "0 1 2 3");
  out.clear();

  r >> Drop<int>(7) >> out;
  EXPECT_STREQ(out.text(), "8 9 10");
  out.clear();

  r >> Take<int>(3) >> out;
  EXPECT_STREQ(out.text(), "1 2 3");
  out.clear();

  r >> Toggle<int,int>(0) >> out;
  EXPECT_STREQ(out.text(), "1 0 1 0 1 0 1 0 1 0");
}

#include "gtest/gtest.h"

#define CONSTRUCTED runAll();
void fatal(const char *msg) { puts(msg); exit(1); }
unsigned int epoch_timestamp() { return time(NULL); }

#include "foundation/rxfusion.h"
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

TEST(OperatorTest, Timed) {
  Doc<int> out;
  Range<int> ra(1,3);
  Range<int> rb(5,8);

  // 80 5
  // 100 1
  // 160 6
  // 200 2
  // 240 7
  // 300 3
  // 320 8

  ((ra >> Iterate<int>(100)) + (rb >> Iterate<int>(80))) >> Sample<int>(140) >> out;
  EXPECT_STREQ(out.text(), "1 7 8");
  out.clear();

  ((ra >> Iterate<int>(100)) + (rb >> Iterate<int>(80))) >> Debounce<int>(30) >> out;
  EXPECT_STREQ(out.text(), "1 6 2 7 8");
}

TEST(OperatorTest, Misc) {
  Doc<int> out;
  Range<int> r(1,10);

  r >> Map<int,int>([](int x)->int { return x/3; }) >> Dedup<int>() >> out;
  EXPECT_STREQ(out.text(), "0 1 2 3");
  out.clear();

  // 100 1 => 0
  // 200 2 => 0 drop = t-100
  // 300 3 => 0 drop = t-200
  // 400 4 => 0 drop = t-300
  // 500 5 => 0
  // 600 6 => 0 drop = t-100
  // 700 7 => 1 
  // 800 8 => 1 drop = t-100
  // 900 9 => 1 drop = t-200
  // 1000 10 => 1 drop = t-300
  r >> Iterate<int>(100) >> Map<int,int>([](int x)->int { return x/7; }) >> Dedup<int>(350) >> out;
  EXPECT_STREQ(out.text(), "0 1 1 2");
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

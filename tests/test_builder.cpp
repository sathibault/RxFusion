#include "gtest/gtest.h"

#define CONSTRUCTED runAll();
void fatal(const char *msg) { puts(msg); exit(1); }
unsigned int epoch_timestamp() { return time(NULL); }

#include "rxfusion/rxfusion.h"
#include "test_scheduler.h"

TEST(BuilderTest, Minimal) {
  Doc<int> out;
  Range<int> r(1,3);

  r >> out;
  EXPECT_STREQ(out.text(), "1 2 3");
}

TEST(BuilderTest, Tuples) {
  Doc<Tuple2<int,int>> out2;
  Doc<Tuple3<int,bool,int>> out3;
  Doc<Tuple4<int,bool,int,bool>> out4;
  Range<int> r(1,3);

  (r ^ (r >> Scale<int>(2))) >> out2;
  EXPECT_STREQ(out2.text(), "(1,2) (2,4) (3,6)");

  (r ^ (r >> IsOver<int>(2)) ^ (r >> Scale<int>(2))) >> out3;
  EXPECT_STREQ(out3.text(), "(1,0,2) (2,0,4) (3,1,6)");

  (r ^ (r >> IsOver<int>(2)) ^ (r >> Scale<int>(2)) ^ (r >> IsAtMost<int>(2))) >> out4;
  EXPECT_STREQ(out4.text(), "(1,0,2,1) (2,0,4,1) (3,1,6,0)");
}

TEST(BuilderTest, Vectors) {
  Doc<Vec<int,2>> out2;
  Doc<Vec<int,3>> out3;
  Doc<Vec<int,4>> out4;
  Range<int> r(1,3);

  (r & (r >> Add<int>(1))) >> out2;
  EXPECT_STREQ(out2.text(), "[1,2] [2,3] [3,4]");

  (r & (r >> Add<int>(1)) & (r >> Add<int>(2))) >> out3;
  EXPECT_STREQ(out3.text(), "[1,2,3] [2,3,4] [3,4,5]");

  (r & (r >> Add<int>(1)) & (r >> Add<int>(2)) & (r >> Add<int>(3))) >> out4;
  EXPECT_STREQ(out4.text(), "[1,2,3,4] [2,3,4,5] [3,4,5,6]");
}

TEST(BuilderTest, Merge) {
  Doc<int> out;
  Range<int> r(1,3);

  (r + (r >> Add<int>(1))) >> out;
  EXPECT_STREQ(out.text(), "1 2 2 3 3 4");

  out.clear();
  (r + (r >> Add<int>(1)) + (r >> Add<int>(2))) >> out;
  EXPECT_STREQ(out.text(), "1 2 3 2 3 4 3 4 5");

  out.clear();
  (r + (r >> Add<int>(1)) + (r >> Add<int>(2)) + (r >> Add<int>(3))) >> out;
  EXPECT_STREQ(out.text(), "1 2 3 4 2 3 4 5 3 4 5 6");
}

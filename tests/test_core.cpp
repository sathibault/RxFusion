#include "gtest/gtest.h"

#define CONSTRUCTED runAll();
void fatal(const char *msg) { puts(msg); exit(1); }
unsigned int epoch_timestamp() { return time(NULL); }

#include "foundation/rxfusion.h"
#include "test_scheduler.h"

TEST(CoreTest, Scan) {
  Doc<int> out;
  Range<int> r(1,3);

  r >> Scan<int,int>([](const int& cur, const int& state) -> int {
      return state+cur;
    }, 0) >> out;
  EXPECT_STREQ(out.text(), "1 3 6");
  out.clear();

  r >> Scan<int,int>([](const int& cur, int& state) {
      state += cur;
    }, [](int& state) {
      state = 0;
    }) >> out;
  EXPECT_STREQ(out.text(), "1 3 6");
  out.clear();

  r >> Scan<int,int>([](const int& cur, int& state) {
      state = cur;
    }) >> out;
  EXPECT_STREQ(out.text(), "1 2 3");
  out.clear();
}

TEST(CoreTest, Map) {
  Doc<int> out;
  Range<int> r(1,3);

  r >> Map<int,int>([](const int& cur) -> int {
      return 2 * cur;
    }) >> out;
  EXPECT_STREQ(out.text(), "2 4 6");
  out.clear();

  r >> Map<int,int>([](const int& cur, int& result) {
      result = 2 * cur;
    }) >> out;
  EXPECT_STREQ(out.text(), "2 4 6");
  out.clear();
}

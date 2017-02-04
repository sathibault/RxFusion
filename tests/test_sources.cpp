#include "gtest/gtest.h"

#define CONSTRUCTED runAll();
void fatal(const char *msg) { puts(msg); exit(1); }
unsigned int epoch_timestamp() { return time(NULL); }

#include "foundation/rxfusion.h"
#include "test_scheduler.h"

TEST(SourceTest, Interval) {
  Interval<int> beat(500);
  Repeat<char> loop('.');
  Doc<int> intout;
  Doc<char> charout;

  beat >> Take<int>(4) >> intout;
  EXPECT_STREQ(intout.text(), "0 1 2 3");

  loop >> Take<char>(5) >> charout;
  EXPECT_STREQ(charout.text(), ". . . . .");
}

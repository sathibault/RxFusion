#include "gtest/gtest.h"

#define CONSTRUCTED runAll();
void fatal(const char *msg) { puts(msg); exit(1); }
unsigned int epoch_timestamp() { return time(NULL); }

#include "rxfusion/rxfusion.h"
#include "test_scheduler.h"

TEST(SourceTest, Interval) {
  Doc<int> out;
  Interval<int> beat(500);

  beat >> Take<int>(4) >> out;
  EXPECT_STREQ(out.text(), "0 1 2 3");
}

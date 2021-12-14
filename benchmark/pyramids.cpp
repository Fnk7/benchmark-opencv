#include "imgproc.hpp"
#include "time.hpp"
#include "benchmark.hpp"

auto pyrDown_func()
{
    return pyrDown;
}

void pyramids()
{
    auto f1 = pyrDown_func();
    TEST(f1(a, ud, w, h, w / 2, h / 2, 1, 0, h), checksum(ud, hw / 4) / hwc);
    TEST(f1(a, ud, w, h, w / 2, h / 2, cn, 0, h), checksum(ud, hw / 4) / hwc);
}


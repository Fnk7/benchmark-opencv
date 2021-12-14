#include "core.hpp"
#include "time.hpp"
#include "benchmark.hpp"

auto magnitude_func()
{
    void (*func)(float *, float *, float *, int) = magnitude;
    return func;
}

void magnitude()
{
    auto f1 = magnitude_func();
    TEST(f1(x, y, fd, hwc), checksum(fd, hwc) / hwc);
}

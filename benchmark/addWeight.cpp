#include "core.hpp"
#include "time.hpp"
#include "benchmark.hpp"

auto addWeight_uchar()
{
    auto func = addWeight<uchar, uchar>;
    return func;
}

auto addWeight_float()
{
    auto func = addWeight<float, float>;
    return func;
}

void addWeight()
{
    auto f1 = addWeight_uchar();
    TEST(f1(a, b, ud, hwc, 0.5, 0.4, 16), checksum(ud, hwc) / hwc);

    auto f2 = addWeight_float();
    TEST(f2(x, y, fd, hwc, 0.6, 0.4, 0), checksum(fd, hwc) / hwc);
}


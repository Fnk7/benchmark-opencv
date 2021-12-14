#include "core.hpp"
#include "time.hpp"
#include "benchmark.hpp"

auto compare_uchar()
{
    auto func = compare<uchar>;
    return func;
}

auto compare_float()
{
    auto func = compare<float>;
    return func;
}

void compare()
{
    auto f1 = compare_uchar();
    TEST(f1(a, b, ud, hwc), checksum(ud, hwc) / hwc);

    auto f2 = compare_float();
    TEST(f2(x, y, ud, hwc), checksum(ud, hwc) / hwc);
}


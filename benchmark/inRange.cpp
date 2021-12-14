#include "core.hpp"
#include "time.hpp"
#include "benchmark.hpp"

auto inRange_uchar()
{
    auto func = inRange<uchar>;
    return func;
}

auto inRange_float()
{
    auto func = inRange<float>;
    return func;
}

void inRange()
{
    auto f1 = inRange_uchar();
    TEST(f1(a, b, c, ud, hw, 1), checksum(ud, hw) / hw);
    TEST(f1(a, b, c, ud, hw, cn), checksum(ud, hw) / hw);

    auto f2 = inRange_float();
    TEST(f2(y, z, x, ud, hw, 1), checksum(ud, hw) / hw);
    TEST(f2(y, z, x, ud, hw, cn), checksum(ud, hw) / hw);
}

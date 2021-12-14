#include "core.hpp"
#include "time.hpp"
#include "benchmark.hpp"

auto normL1_uchar()
{
    auto func = normL1<uchar, int>;
    return func;
}

auto normL1_float()
{
    auto func = normL1<float, float>;
    return func;
}

void normL1()
{
    inRange(a, b, c, ud, hw, cn);
    {
        int result;
        auto f1 = normL1_uchar();
        TEST(f1(a, nullptr, &(result = 0), hw, cn), (double)result / hwc);
        TEST(f1(a, ud, &(result = 0), hw, cn), (double)result / hwc);
    }
    {
        float result;
        auto f2 = normL1_float();
        TEST(f2(x, nullptr, &(result = 0), hw, cn), result / hwc);
        TEST(f2(x, ud, &(result = 0), hw, cn), result / hwc);
    }
}

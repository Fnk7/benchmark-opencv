#include "core.hpp"
#include "time.hpp"
#include "benchmark.hpp"

auto normL2_uchar()
{
    auto func = normL2<uchar, int>;
    return func;
}

auto normL2_float()
{
    auto func = normL2<float, float>;
    return func;
}

void normL2()
{
    inRange(a, b, c, ud, hw, cn);

    {
        auto f1 = normL2_uchar();
        int result;
        // hw should be lower than 10922, otherwise overflow, but not influence time measurement
        TEST(f1(a, nullptr, &(result = 0), hw, cn), (double)result / hwc);
        TEST(f1(a, ud, &(result = 0), hw, cn), (double)result / hwc);
    }
    {
        float result;
        auto f2 = normL2_float();
        TEST(f2(x, nullptr, &(result = 0), hw, cn), result / hwc);
        TEST(f2(x, ud, &(result = 0), hw, cn), result / hwc);
    }
}

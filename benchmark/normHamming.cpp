#include "core.hpp"
#include "time.hpp"
#include "benchmark.hpp"

auto normHamming_func()
{
    void (*func)(uchar *, uchar *, int *, int) = normHamming;
    return func;
}

void normHamming()
{
    int result;
    auto f1 = normHamming_func();
    TEST(f1(a, nullptr, &(result = 0), hw), result / hw);
    TEST(f1(a, b, &(result = 0), hw), result / hw);
}

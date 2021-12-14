#include "core.hpp"
#include "time.hpp"
#include "benchmark.hpp"

auto transpose_uchar_1()
{
    auto func = transpose<uchar, 1>;
    return func;
}

auto transpose_uchar_3()
{
    auto func = transpose<uchar, 3>;
    return func;
}

auto transpose_float_1()
{
    auto func = transpose<float, 1>;
    return func;
}

auto transpose_float_3()
{
    auto func = transpose<float, 3>;
    return func;
}

void transpose()
{
    auto f1 = transpose_uchar_1();
    TEST(f1(a, ud, h, w), checksum(ud, h * cn));
    auto f2 = transpose_uchar_3();
    TEST(f2(a, ud, h, w), checksum(ud, h * cn));
    auto f3 = transpose_float_1();
    TEST(f3((uchar *)x, (uchar *)fd, h, w), checksum(fd, h * cn));
    auto f4 = transpose_float_3();
    TEST(f4((uchar *)x, (uchar *)fd, h, w), checksum(fd, h * cn));
}

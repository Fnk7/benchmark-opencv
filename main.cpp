#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include "benchmark/benchmark.hpp"

using namespace std;

void init(uchar *a, int len, int step, uchar base)
{
    for (int i = 0; i < len; i += 16)
    {
        for (int j = 0; j < 15; j++)
            a[i + j] = (uchar)(((i + j) * step + base) & 255);
        a[i + 15] = (uchar)((-(i * step + base)) & 255);
    }
}

void init(float *x, int len, float step, float base)
{
    for (int i = 0; i < len; i++)
    {
        x[i] = cos(step * i) + base;
    }
}

uchar *a, *b, *c, *ud;
float *x, *y, *z, *fd;

#define TEST(x) {cout << #x << ":" << endl; x();}

int main()
{
    const int h = 540;
    const int w = 960;
    const int cn = 3;
    const int hw = h * w;
    const int hwc = hw * cn;
    double result;
    int result2;
    a = (uchar *)aligned_alloc(32, hwc);
    b = (uchar *)aligned_alloc(32, hwc);
    c = (uchar *)aligned_alloc(32, hwc);
    ud = (uchar *)aligned_alloc(32, hwc * 4);
    x = (float *)aligned_alloc(32, hwc * sizeof(float));
    y = (float *)aligned_alloc(32, hwc * sizeof(float));
    z = (float *)aligned_alloc(32, hwc * sizeof(float));
    fd = (float *)aligned_alloc(32, hwc * sizeof(float));
    init(a, hwc, 2, 11);
    init(b, hwc, 3, 13);
    init(c, hwc, 5, 17);
    init(x, hwc, 1, 0.03);
    init(y, hwc, 2, 0.05);
    init(z, hwc, 3, 0.07);

    TEST(addWeight);
    TEST(compare);
    TEST(inRange);
    TEST(normHamming);
    TEST(normL1);
    TEST(normL2);
    TEST(magnitude);
    TEST(transpose);

    TEST(pyramids);
    TEST(morph);

    TEST(filter);

    // // pyrUp(a, ud, w, h, w * 2, h * 2, 1);
    // // checksum(ud, hw * 4, &result);
    // // cout << "pyrUp\t\t" << result << endl;
    // // pyrUp(a, ud, w, h, w * 2, h * 2, cn);
    // // checksum(ud, hwc * 4, &result);
    // // cout << "pyrUp\t\t" << result << endl;

    return 0;
}

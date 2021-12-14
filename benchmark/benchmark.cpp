#include "benchmark.hpp"
#include <algorithm>

void dummy(uchar *ud, float *fd) {}

double checksum(uchar *a, int len)
{
    double _result = 0;
    for (int i = 0; i < len; i += 2048)
    {
        int result = 0;
        for (int j = i; j < std::min(len, i + 2048); j++)
            result += a[j];
        _result += result;
    }
    return _result;
}

double checksum(float *x, int len)
{
    double _result = 0;
    for (int i = 0; i < len; i += 2048)
    {
        float result = 0;
        for (int j = i; j < std::min(len, i + 2048); j++)
            result += x[j];
        _result += result;
    }
    return _result;
}

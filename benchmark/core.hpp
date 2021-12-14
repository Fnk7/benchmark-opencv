#include "common.hpp"
#include <cmath>

template <typename ST, typename DT>
DT inline s2d_cast(ST value);

template <>
uchar inline s2d_cast<float, uchar>(float value)
{
    int v = (int)(value + 0.5);
    return value < 0 ? 0 : v > 255 ? 255 : value;
}

template <>
float inline s2d_cast<float, float>(float value) { return value; }

template <typename ST, typename DT>
void addWeight(ST *src1, ST *src2, DT *dst, int width, float alpha, float beta, float gamma)
{
    // width = 1080 * 1920 * 3
    // O3, no difference between "x++" "++x"
    for (int x = 0; x < width; x++)
    {
        float value = (float)(src1[x]) * alpha + (float)(src2[x]) * beta + gamma;
        dst[x] = s2d_cast<float, DT>(value);
    }
}

template <typename ST>
void compare(ST *src1, ST *src2, uchar *dst, int width)
{
    // width = 1080 * 1920 * 3
    for (int x = 0; x < width; x++)
    {
        dst[x] = (uchar) - (src1[x] >= src2[x]);
    }
}

template <typename ST>
void inRange(ST *src1, ST *src2, ST *src3, uchar *dst, int width, int cn)
{
    // width = 342 (for locality if cn > 1)
    // part 1
    uchar *mbuf = dst;
    if (cn != 1)
        mbuf = new uchar[width * cn];
    for (int x = 0; x < width * cn; x++)
        mbuf[x] = (uchar) - (src2[x] <= src1[x] && src1[x] <= src3[x]);
    // part 2
    int i, j, k;
    if (cn == 3)
        for (i = j = 0; i < width; i++, j += cn)
            dst[i] = mbuf[j] & mbuf[j + 1] & mbuf[j + 2];
    else
        for (i = j = 0; i < width; i++, j += cn)
        {
            dst[i] = mbuf[j];
            for (k = 1; k < cn; k++)
                dst[i] &= mbuf[j + k];
        }
    if (mbuf != dst)
        delete mbuf;
}

static void normHamming(uchar *a, uchar *b, int *_result, int n)
{
    static const uchar popCountTable[256] = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};
    // n = 1080 * 1920
    int result = *_result;
    if (b)
    {
        for (int i = 0; i < n; i++)
            result += popCountTable[a[i] ^ b[i]];
    }
    else
    {
        for (int i = 0; i < n; i++)
            result += popCountTable[a[i]];
    }

    *_result = result;
}

template <typename ST, typename DT>
void normL1(ST *src, uchar *mask, DT *_result, int len, int cn)
{
    // 255 * len * cn < max int value
    // len = 1080 * 1920
    DT result = *_result;
    if (mask)
    {
        for (int i = 0; i < len; i++, src += cn)
        {
            if (mask[i])
                for (int k = 0; k < cn; k++)
                    result += src[k];
        }
    }
    else
    {
        for (int i = 0; i < len * cn; i++)
            result += src[i];
    }
    *_result = result;
}

template <typename ST, typename DT>
void normL2(ST *src, uchar *mask, DT *_result, int len, int cn)
{
    // 255 * 255 * len * cn < max int value
    // len = 10922 (if is_same<ST, uchar> and is_same<DT, int>)
    DT result = *_result;
    if (mask)
    {
        for (int i = 0; i < len; i++, src += cn)
        {
            if (mask[i])
                for (int k = 0; k < cn; k++)
                {
                    ST v = src[k];
                    result += (DT)v * v;
                }
        }
    }
    else
    {
        for (int i = 0; i < len * cn; i++)
        {
            ST v = src[i];
            result += (DT)v * v;
        }
    }
    *_result = result;
}

static void magnitude(float *x, float *y, float *mag, int len)
{
    // len = 1080 * 1920 * channel number
    for (int i = 0; i < len; i++)
        mag[i] = std::sqrt(x[i] * x[i] + y[i] * y[i]);
}

template <typename ST, int cn>
void transpose(uchar *src, uchar *dst, int width, int height)
{
    static_assert(cn >= 1 && cn <= 8, "channel number invalid");
    // width = 1920
    // height = 1080
    int sstep = width * cn * sizeof(ST);
    int dstep = height * cn * sizeof(ST);
    for (int i = 0; i < width; i++)
    {
        ST *d0 = (ST *)(dst + dstep * i);
        for (int j = 0; j < height; j++)
        {
            ST *s0 = (ST *)(src + i * cn * sizeof(ST) + j * sstep);
            for (int k = 0; k < cn; k++)
                d0[j * cn + k] = s0[k];
        }
    }
}

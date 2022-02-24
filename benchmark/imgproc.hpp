#include "common.hpp"
#include <algorithm>

static int inline borderInterpolate(int p, int len)
{
    if ((unsigned)p < (unsigned)len)
        ;
    else
    {
        int delta = (0 == 0);
        if (len == 1)
            return 0;
        do
        {
            if (p < 0)
                p = -p - 1 + delta;
            else
                p = len - 1 - (p - len) - delta;
        } while ((unsigned)p >= (unsigned)len);
    }
    return p;
}
static void pyrUp(uchar *src, uchar *dst, int src_w, int src_h, int dst_w, int dst_h, int cn)
{
    // src_w = 1920
    // src_h = 1080
    // assert(src_w * 2 == dst_w && src_h * 2 == dst_h);
    // cn = 3
    const int PU_SZ = 3;

    // using WT = int;
    // using T = uchar;

    int bufstep = (dst_w + 1) * cn;
    // assert(bufstep % 16 == 0);
    int *buf = new int[bufstep * PU_SZ + 16];
    int *dtab = new int[src_w * cn];
    int *rows[PU_SZ];
    uchar *dsts[2];
    int k, x, sy0 = -PU_SZ / 2, sy = sy0;

    int dst_cols = dst_w;
    src_w *= cn;
    dst_w *= cn;

    for (x = 0; x < src_w; x++)
        dtab[x] = (x / cn) * 2 * cn + x % cn;

    // large loop
    for (int y = 0; y < src_h; y++)
    {
        uchar *dst0 = dst + y * 2 * dst_w;
        uchar *dst1 = dst + std::min(y * 2 + 1, dst_h - 1) * dst_w;
        int *row0, *row1, *row2;
        // small loop
        for (; sy <= y + 1; sy++)
        {
            int *row = buf + ((sy - sy0) % PU_SZ) * bufstep;
            int _sy = borderInterpolate(sy * 2, src_h * 2) / 2;
            _sy = sy * 2 < src_h * 2 ? sy : 0;
            const uchar *src = src + _sy * src_w;
            for (x = 0; x < cn; x++)
            {
                int dx = dtab[x];
                int t0 = src[x] * 6 + src[x + cn] * 2;
                int t1 = (src[x] + src[x + cn]) * 4;
                row[dx] = t0;
                row[dx + cn] = t1;
                dx = dtab[src_w - cn + x];
                int sx = src_w - cn + x;
                t0 = src[sx - cn] + src[sx] * 7;
                t1 = src[sx] * 8;
                row[dx] = t0;
                row[dx + cn] = t1;
            }
            // horizontal
            // large loop
            for (x = cn; x < src_w - cn; x++)
            {
                int dx = dtab[x];
                int t0 = src[x - cn] + src[x] * 6 + src[x + cn];
                int t1 = (src[x] + src[x + cn]) * 4;
                row[dx] = t0;
                row[dx + cn] = t1;
            }
        }
        // vertical
        for (k = 0; k < PU_SZ; k++)
            rows[k] = buf + ((y - PU_SZ / 2 + k - sy0) % PU_SZ) * bufstep;
        row0 = rows[0];
        row1 = rows[1];
        row2 = rows[2];
        dsts[0] = dst0;
        dsts[1] = dst1;
        x = 0;
        for (; x < dst_w; x++)
        {
            uchar t1 = (uchar)((((row1[x] + row2[x]) * 4) + (1 << 5)) >> 6);
            uchar t0 = (uchar)(((row0[x] + row1[x] * 6 + row2[x]) + (1 << 5)) >> 6);
            dst1[x] = t1;
            dst0[x] = t0;
        }
    }
    delete buf;
    delete dtab;
}

static void pyrDown(uchar *_src, uchar *_dst, int src_w, int src_h, int dst_w, int dst_h, int cn, int range_start, int range_end)
{
    // assert(src_w / 2 == dst_w && src_h / 2 == dst_h);
    // assert(cn == 3 || cn == 1);
    const int PD_SZ = 5;
    int *tabL = new int[cn * (PD_SZ + 2)];
    int *tabR = new int[cn * (PD_SZ + 2)];
    int width0 = std::min((src_w - PD_SZ / 2 - 1) / 2 + 1, dst_w);
    for (int x = 0; x <= PD_SZ + 1; x++)
    {
        int sx0 = borderInterpolate(x - PD_SZ / 2, src_w) * cn;
        int sx1 = borderInterpolate(x + width0 * 2 - PD_SZ / 2, src_w) * cn;
        for (int k = 0; k < cn; k++)
        {
            tabL[x * cn + k] = sx0 + k;
            tabR[x * cn + k] = sx1 + k;
        }
    }
    // can be openmp parallel
    // assert(range_start >= 0 && range_start < dst_h);
    // assert(range_end > 0 && range_end <= dst_h);
    int bufstep = dst_w * cn;
    // assert(bufstep % 16 == 0);
    int *buf = new int[bufstep * PD_SZ + 16];
    int *rows[PD_SZ];

    int sy0 = -PD_SZ / 2, sy = range_start * 2 + sy0;
    width0 = std::min((src_w - PD_SZ / 2 - 1) / 2 + 1, dst_w);
    src_w *= cn;
    dst_w *= cn;
    width0 *= cn;
    // large loop: height
    for (int y = range_start; y < range_end; y++)
    {
        uchar *dst = _dst + dst_w * y;
        int *row0, *row1, *row2, *row3, *row4;

        // fill the ring buffer (horizontal convolution and decimation)
        int sy_limit = y * 2 + 2;
        for (; sy <= sy_limit; sy++)
        {
            int *row = buf + ((sy - sy0) % PD_SZ) * bufstep;
            int _sy = borderInterpolate(sy, src_h);
            const uchar *src = _src + src_w * _sy;
            do
            {
                int x = 0;
                for (; x < cn; x++)
                {
                    row[x] = src[tabL[x + cn * 2]] * 6 + (src[tabL[x + cn]] + src[tabL[x + cn * 3]]) * 4 +
                             src[tabL[x]] + src[tabL[x + cn * 4]];
                }
                if (x == dst_w)
                    break;
                if (cn == 1)
                {
                    // large loop: width
                    for (; x < width0; x++)
                        row[x] = src[x * 2] * 6 + (src[x * 2 - 1] + src[x * 2 + 1]) * 4 +
                                 src[x * 2 - 2] + src[x * 2 + 2];
                }
                else if (cn == 3)
                {
                    // large loop: width
                    for (; x < width0; x += 3)
                    {
                        const uchar *s = src + x * 2;
                        int t0 = s[0] * 6 + (s[-3] + s[3]) * 4 + s[-6] + s[6];
                        int t1 = s[1] * 6 + (s[-2] + s[4]) * 4 + s[-5] + s[7];
                        int t2 = s[2] * 6 + (s[-1] + s[5]) * 4 + s[-4] + s[8];
                        row[x] = t0;
                        row[x + 1] = t1;
                        row[x + 2] = t2;
                    }
                }

                // tabR
                // large loop: width
                for (int x_ = 0; x < dst_w; x++, x_++)
                {
                    row[x] = src[tabR[x_ + cn * 2]] * 6 + (src[tabR[x_ + cn]] + src[tabR[x_ + cn * 3]]) * 4 +
                             src[tabR[x_]] + src[tabR[x_ + cn * 4]];
                }
            } while (0);
        }

        // do vertical convolution and decimation and write the result to the destination image
        for (int k = 0; k < PD_SZ; k++)
            rows[k] = buf + ((y * 2 - PD_SZ / 2 + k - sy0) % PD_SZ) * bufstep;
        row0 = rows[0];
        row1 = rows[1];
        row2 = rows[2];
        row3 = rows[3];
        row4 = rows[4];
        // larget loop: width
        for (int x = 0; x < dst_w; x++)
            dst[x] = (uchar)(((row2[x] * 6 + (row1[x] + row3[x]) * 4 + row0[x] + row4[x]) + (1 << 7)) >> 7);
    }
}

template <typename ST, int cn>
void morph_row_filter(ST *src, ST *dst, int width)
{
    // src and dst is a row of Mat image
    // width = 1920
    // cn = 3
    int ksize = 3;
    int i, j, k, _ksize = ksize * cn;
    const ST *S = (const ST *)src;
    ST *D = (ST *)dst;
    int i0 = 0;
    width *= cn;
    for (k = 0; k < cn; k++, S++, D++)
    {
        i = i0;
#ifdef SCALAR_OPT
        for (; i <= width - cn * 2; i += cn * 2)
        {
            const ST *s = S + i;
            ST m = s[cn];
            for (j = cn * 2; j < _ksize; j += cn)
                m = std::min(m, s[j]); // for dilate, use max
            D[i] = std::min(m, s[0]);
            D[i + cn] = std::min(m, s[j]);
        }
#endif
        for (; i < width; i += cn)
        {
            const ST *s = S + i;
            ST m = s[0];
            for (j = cn; j < _ksize; j += cn)
                m = std::min(m, s[j]);
            D[i] = m;
        }
    }
}

template <typename ST>
static void morph_col_filter(ST **_src, ST *dst, int dststep, int count, int width)
{
    // dststep = 1920 * 3
    // count = 4
    // width = 1920 * 3
    const int ksize = 3;
    int i, k, _ksize = ksize;
    const ST **src = (const ST **)_src;
    ST *D = (ST *)dst;
    int i0 = 0;
    for (; _ksize > 1 && count > 1; count -= 2, D += dststep * 2, src += 2)
    {
        i = i0;
        for (; i < width; i++)
        {
            ST s0 = src[1][i];
            // for (k = 2; k < _ksize; k++)
            s0 = std::min(s0, src[2][i]);
            D[i] = std::min(s0, src[0][i]);
            D[i + dststep] = std::min(s0, src[k][i]);
        }
    }
    for (; count > 0; count--, D += dststep, src++)
    {
        i = i0;
        for (; i < width; i++)
        {
            ST s0 = src[0][i];
            // for (k = 1; k < _ksize; k++)
            s0 = std::min(s0, src[1][i]);
            s0 = std::min(s0, src[2][i]);
            D[i] = s0;
        }
    }
}

template <typename ST, int cn, int ksize>
void row_filter(ST *src, ST *dst, ST *kernel, int width)
{
    int _ksize = ksize;
    const ST *kx = kernel;
    const ST *S = (ST *)src;
    ST *D = (ST *)dst;
    int i, k;
    width *= cn;
#ifdef ORIGIN
    for (i = 0; i < width; i++)
    {
        S = (const ST *)src + i;
        ST s0 = kx[0] * S[0];
        for (k = 1; k < _ksize; k++)
        {
            S += cn;
            s0 += kx[k] * S[0];
        }
        D[i] = s0;
    }
#else
    for (i = 0; i < width; i++)
    {
        D[i] = S[i] * kx[0];
    }
    for (k = 1; k < _ksize; k++)
    {
        for (i = 0; i < width; i++)
        {
            D[i] += S[i] * kx[k];
        }
    }
#endif
}

template <typename ST, int ksize>
void column_filter(const ST **src, ST *dst, ST *kernel, ST delta, int count, int width)
{
    const ST *ky = kernel;
    ST _delta = delta;
    int _ksize = ksize;
    int i, k;
    int dststep = width; // no padding
    for (; count--; dst += dststep, src++)
    {
        ST *D = (ST *)dst;
#ifdef ORIGIN
        for (i = 0; i < width; i++)
        {
            ST s0 = ky[0] * ((const ST *)src[0])[i] + _delta;
            for (k = 1; k < _ksize; k++)
                s0 += ky[k] * ((const ST *)src[k])[i];
            D[i] = s0;
        }
#else
        for (i = 0; i < width; i++)
            D[i] = _delta;
        for (k = 0; k < _ksize; k++)
        {
            const ST *src_k = (const ST *)src[k];
            for (i = 0; i < width; i++)
                D[i] += ky[k] * src_k[i];
        }
#endif
    }
}

template <typename ST, int ksize, bool symmetrical>
void symm_column_filter(const ST **src, ST *dst, ST *kernel, ST delta, int count, int width)
{
    int ksize2 = ksize / 2;
    const ST *ky = kernel + ksize2;
    int i, k;
    ST _delta = delta;
    src += ksize2;
    int dststep = width; // no padding
    if (symmetrical)
    {
        for (; count--; dst += dststep, src++)
        {
            ST *D = (ST *)dst;
#ifdef ORIGIN
            for (i = 0; i < width; i++)
            {
                ST s0 = ky[0] * ((const ST *)src[0])[i] + _delta;
                for (k = 1; k <= ksize2; k++)
                    s0 += ky[k] * (((const ST *)src[k])[i] + ((const ST *)src[-k])[i]);
                D[i] = s0;
            }
#else
            for (i = 0; i < width; i++)
                D[i] = ky[0] * ((const ST *)src[0])[i] + _delta;
            for (k = 1; k <= ksize2; k++)
            {
                const ST *src_k = (const ST *)src[k];
                const ST *src_mk = (const ST *)src[-k];
                for (i = 0; i < width; i++)
                    D[i] += ky[k] * (src_k[i] + src_mk[i]);
            }
#endif
        }
    }
    else
    {
        for (; count--; dst += dststep, src++)
        {
            ST *D = (ST *)dst;
#ifdef ORIGIN
            for (i = 0; i < width; i++)
            {
                ST s0 = _delta;
                for (k = 1; k <= ksize2; k++)
                    s0 += ky[k] * (((const ST *)src[k])[i] - ((const ST *)src[-k])[i]);
                D[i] = s0;
            }
#else
            for (i = 0; i < width; i++)
                D[i] = _delta;
            for (k = 1; k <= ksize2; k++)
            {
                const ST *src_k = (const ST *)src[k];
                const ST *src_mk = (const ST *)src[-k];
                for (i = 0; i < width; i++)
                    D[i] += ky[k] * (src_k[i] - src_mk[i]);
            }
#endif
        }
    }
}

#ifdef USE_INTRINSICS
#include <arm_sve.h>
template <int cn, int ksize>
void row_filter_intrinsics(const float *_src, float *_dst, float *kernel, int width)
{
    int _ksize = ksize;
    const float *src0 = (const float *)_src;
    float *dst = (float *)_dst;
    const float *_kx = kernel;
    int i = 0, k;
    width *= cn;

    svbool_t pg = svwhilelt_b32(i, width);
    while (svptest_any(svptrue_b32(), pg))
    {
        const float *src = src0 + i;
        svfloat32_t s0 = svld1(pg, src);
        s0 = svmul_x(pg, s0, _kx[0]);
        src += cn;
        for (k = 1; k < _ksize; k++, src += cn)
        {
            svfloat32_t sk = svld1(pg, src);
            s0 = svmla_x(pg, s0, sk, _kx[k]);
        }
        svst1(pg, dst + i, s0);
        i += svcntw();
        pg = svwhilelt_b32(i, width);
    }
}

template <int cn, int ksize, bool symmetrical>
void symm_column_filter_intrinsics(const float **_src, float *_dst, float *kernel, int width)
{

    int ksize2 = ksize / 2;
    const float *ky = kernel;
    int i = 0, k;
    const float **src = (const float **)_src;
    float *dst = (float *)_dst;

    svfloat32_t d4 = svdup_f32(delta);
    if (symmetrical)
    {
        svbool_t pg = svwhilelt_b32(i, width);
        while (svptest_any(svptrue_b32(), pg))
        {
            svfloat32_t s0 = svld1(pg, src[0] + i);
            s0 = svmla_x(pg, d4, s0, ky[0]);
            for (k = 1; k <= ksize2; k++)
            {
                svfloat32_t sk = svld1(pg, src[k] + i);
                svfloat32_t smk = svld1(pg, src[-k] + i);
                sk = svadd_x(pg, sk, smk);
                s0 = svmla_x(pg, s0, sk, ky[k]);
            }
            svst1(pg, dst + i, s0);
            i += svcntw();
            pg = svwhilelt_b32(i, width);
        }
    }
    else
    {
        svbool_t pg = svwhilelt_b32(i, width);
        while (svptest_any(svptrue_b32(), pg))
        {
            svfloat32_t s0 = d4;
            for (k = 1; k <= ksize2; k++)
            {
                svfloat32_t sk = svld1(pg, src[k] + i);
                svfloat32_t smk = svld1(pg, src[-k] + i);
                sk = svsub_x(pg, sk, smk);
                s0 = svmla_x(pg, s0, sk, ky[k]);
            }
            svst1(pg, dst + i, s0);
            i += svcntw();
            pg = svwhilelt_b32(i, width);
        }
    }
}
#endif
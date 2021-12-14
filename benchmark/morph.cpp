#include "imgproc.hpp"
#include "time.hpp"
#include "benchmark.hpp"

auto morph_row()
{
    return morph_row_filter<uchar, 1>;
}

auto morph_row2()
{
    return morph_row_filter<uchar, 3>;
}

auto morph_col()
{
    return morph_col_filter<uchar>;
}

auto morph_row_f()
{
    return morph_row_filter<float, 1>;
}

auto morph_row2_f()
{
    return morph_row_filter<float, 3>;
}

auto morph_col_f()
{
    return morph_col_filter<float>;
}

void morph()
{
    {
        auto f1 = morph_row();
        TEST(f1(a, ud, w), checksum(ud, w) / w);
        auto f2 = morph_row2();
        TEST(f2(a, ud, w), checksum(ud, w * cn) / (w * cn));

        uchar *morph_rows[h];
        for (int i = 0; i < h; i++)
            morph_rows[i] = a + i * w * cn;
        auto f3 = morph_col();
        TEST(f3(morph_rows, ud, w * cn, h - 2, w * cn), checksum(ud, w * cn) / (w * cn));
    }
    {
        auto f1 = morph_row_f();
        TEST(f1(x, fd, w), checksum(fd, w) / w);
        auto f2 = morph_row2_f();
        TEST(f2(x, fd, w), checksum(fd, w * cn) / (w * cn));

        float *morph_rows[h];
        for (int i = 0; i < h; i++)
            morph_rows[i] = x + i * w * cn;
        auto f3 = morph_col_f();
        TEST(f3(morph_rows, fd, w * cn, h - 2, w * cn), checksum(fd, w * cn) / (w * cn));
    }
}

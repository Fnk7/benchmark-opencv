#include "imgproc.hpp"
#include "time.hpp"
#include "benchmark.hpp"

auto filter_row()
{
    return row_filter<float, 1, 3>;
}

auto filter_row2()
{
    return row_filter<float, 1, 5>;
}

auto filter_col()
{
    return column_filter<float, 3>;
}

auto filter_col2()
{
    return column_filter<float, 5>;
}

auto filter_symm_col()
{
    return symm_column_filter<float, 3, true>;
}

auto filter_symm_col2()
{
    return symm_column_filter<float, 3, false>;
}

auto filter_symm_col3()
{
    return symm_column_filter<float, 5, true>;
}

auto filter_symm_col4()
{
    return symm_column_filter<float, 5, false>;
}



void filter()
{
    {
        float kernel3[3] = {1, 2, 1};
        float kernel5[5] = {1, 2, 3, 2, 1};
        auto f1 = filter_row();
        TEST(f1(x, fd, kernel3, w), checksum(fd, w) / w);
        auto f2 = filter_row2();
        TEST(f2(x, fd, kernel5, w), checksum(fd, w) / w);

        float delta = 1.0;
        const float *rows[h];
        for (int i = 0; i < h; i++)
            rows[i] = x + i * w * cn;
        auto f3 = filter_col();
        TEST(f3(rows, fd, kernel3, delta, h - 2, w * cn), checksum(fd, w * cn) / (w * cn));
        auto f4 = filter_col2();
        TEST(f4(rows, fd, kernel5, delta, h - 4, w * cn), checksum(fd, w * cn) / (w * cn));
        auto f5 = filter_symm_col();
        TEST(f5(rows, fd, kernel3, delta, h - 2, w * cn), checksum(fd, w * cn) / (w * cn));
        auto f6 = filter_symm_col2();
        TEST(f6(rows, fd, kernel3, delta, h - 2, w * cn), checksum(fd, w * cn) / (w * cn));
        auto f7 = filter_symm_col3();
        TEST(f7(rows, fd, kernel5, delta, h - 4, w * cn), checksum(fd, w * cn) / (w * cn));
        auto f8 = filter_symm_col4();
        TEST(f8(rows, fd, kernel5, delta, h - 4, w * cn), checksum(fd, w * cn) / (w * cn));
    }
}

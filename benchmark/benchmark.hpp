#include "common.hpp"

const int h = 540;
const int w = 960;
const int cn = 3;
const int hw = h * w;
const int hwc = hw * cn;

extern uchar *a, *b, *c, *ud;
extern float *x, *y, *z, *fd;

void dummy(uchar *ud, float *fd);

double checksum(uchar *a, int len);
double checksum(float *a, int len);

void addWeight();
void compare();
void inRange();
void normHamming();
void normL1();
void normL2();
void magnitude();
void transpose();

void pyramids();
void morph();

void filter();

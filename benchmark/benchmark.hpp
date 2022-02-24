#include "common.hpp"

#ifndef INPUT_WIDTH
#define INPUT_WIDTH 1920
#endif

#ifndef INPUT_HEIGHT
#define INPUT_HEIGHT 1080
#endif

const int h = INPUT_HEIGHT;
const int w = INPUT_WIDTH;
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

#include <chrono>
#include <iostream>

#define START auto start = std::chrono::system_clock::now();
#define END auto end = std::chrono::system_clock::now();
#define TIME std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
#define REPORT(value) {std::cout << "    " << TIME << "\t" << (value) << std::endl;}
#define TEST(x, value) {START for (int i = 0; i < 100; i++) {x; dummy(ud, fd);} END REPORT(value)}


#ifndef COMPUTE_H
#define COMPUTE_H

#include <boost/align/aligned_allocator.hpp>
#include <cmath>
#include <vector>

template <typename T>
using aligned_allocator = boost::alignment::aligned_allocator<T, 64>;
template <typename T>
using aligned_vector = std::vector<T, aligned_allocator<T>>;

constexpr float PI = 3.14159265358979323846264338327950288419716939f;
constexpr float TWO_PI = 2 * PI;

float compute(float alpha, float beta, aligned_vector<float> seed_x,
              aligned_vector<float> seed_y, int num_iterations,
              float threshold);
#endif

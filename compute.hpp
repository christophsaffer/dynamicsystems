#ifndef COMPUTE_H
#define COMPUTE_H

#include <cmath>
#include <vector>
#include <boost/align/aligned_allocator.hpp>

template <typename T>
using aligned_allocator = boost::alignment::aligned_allocator<T, 64>;
template <typename T>
using aligned_vector = std::vector<T, aligned_allocator<T>>;

constexpr float PI = 3.141592635;
constexpr float TWO_PI = 6.28318530718;  // 2 * PI;

float compute(float alpha, float beta, aligned_vector<float> seed_x,
          aligned_vector<float> seed_y, int num_iterations, float threshold);
#endif

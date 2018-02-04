#ifndef COMPUTE_H
#define COMPUTE_H

#include <boost/align/aligned_allocator.hpp>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include <png.h>
#include "colormaps.hpp"
#include "picture.hpp"

template <typename T>
using aligned_allocator = boost::alignment::aligned_allocator<T, 64>;
template <typename T>
using aligned_vector = std::vector<T, aligned_allocator<T>>;

constexpr float PI = 3.14159265358979323846264338327950288419716939f;
constexpr float TWO_PI = 2 * PI;

float compute(float alpha, float beta, aligned_vector<float> seed_x,
              aligned_vector<float> seed_y, int num_iterations,
              float threshold);

void compute_all(
  int num_iterations,
  float threshold,
  float alphamin,
  float alphamax,
  int alpha_num_intervals,
  float betamin,
  float betamax,
  int beta_num_intervals,
  int num_seedpoints,
  bool output_csv,
  std::vector<float> seedpoints
  );

#endif

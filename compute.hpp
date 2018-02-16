#ifndef COMPUTE_H
#define COMPUTE_H

#include <vector>

#include <boost/align/aligned_allocator.hpp>

template <typename T>
using aligned_allocator = boost::alignment::aligned_allocator<T, 64>;
template <typename T>
using aligned_vector = std::vector<T, aligned_allocator<T>>;

float compute(float alpha, float beta, const aligned_vector<float>& seed_x,
              const aligned_vector<float>& seed_y, int num_iterations,
              float threshold);

void compute_all(int num_iterations, float threshold, float alphamin,
                 float alphamax, int alpha_num_intervals, float betamin,
                 float betamax, int beta_num_intervals, int num_seedpoints,
                 bool output_csv, std::vector<float> seedpoints);

#endif  // COMPUTE_H

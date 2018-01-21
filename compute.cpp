#include "compute.hpp"

float compute(float alpha, float beta, aligned_vector<float> seed_x,
          aligned_vector<float> seed_y, int num_iterations, float threshold) {
  int num_seeds = seed_x.size();
  assert(seed_y.size() == num_seeds);

  aligned_vector<float> x = seed_x;
  aligned_vector<float> y = seed_y;

  float* xp = x.data();
  float* yp = y.data();

  float d = 0.0;

  for (int i = 0; i < num_iterations && d < threshold ; i++) {
#pragma omp simd aligned(xp, yp : 64)
    for (int s = 0; s < num_seeds; s++) {
      yp[s] = yp[s] + beta * std::sin(TWO_PI * xp[s]);
    }
#pragma omp simd aligned(xp, yp : 64)
    for (int s = 0; s < num_seeds; s++) {
      xp[s] = xp[s] + alpha * std::sin(TWO_PI * yp[s]);
    }

    for (int s = 0; s < num_seeds; s++) {
      d = std::max(d, std::abs(yp[s]));
    }
  }

  return d;
}

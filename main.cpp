#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include <boost/align/aligned_allocator.hpp>

template <typename T>
using aligned_allocator = boost::alignment::aligned_allocator<T, 64>;
template <typename T>
using aligned_vector = std::vector<T, aligned_allocator<T>>;

constexpr float PI = 3.141592635;
constexpr float TWO_PI = 6.28318530718;  // 2 * PI;

// return gray-value of pixel[alpha,beta], between 0..255
int pixel(float alpha, float beta, aligned_vector<float> seed_x,
          aligned_vector<float> seed_y, int num_iterations) {
  int num_seeds = seed_x.size();
  assert(seed_y.size() == num_seeds);

  aligned_vector<float> x = seed_x;
  aligned_vector<float> y = seed_y;

  float* xp = x.data();
  float* yp = y.data();

  float d = 0.0;

  for (int i = 0; i < num_iterations && d < 1; i++) {
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

  if (d > 1) {
    return 255;
  } else {
    return (int)254 * d;
  }
}

int main(int argc, char* argv[]) {
  // get arguments from CLI
  if (argc != 4) {
    std::cerr << "usage: " << argv[0]
              << " <num_iterations> <num_alpha_intervals> <num_beta_intervals>"
              << std::endl;
    return -1;
  }
  // Default parameters
  int num_iterations = 100;
  if (!(std::istringstream(argv[1]) >> num_iterations) ||
      !(num_iterations > 0)) {
    std::cerr << "Number of Iterations must be positive Integer!" << std::endl;
    return -1;
  }

  float alphamin = 0;
  float alphamax = 2;
  int alpha_num_intervals = 600;
  if (!(std::istringstream(argv[2]) >> alpha_num_intervals) ||
      !(alpha_num_intervals > 0)) {
    std::cerr << "Number of alpha-intervals must be positive Integer!"
              << std::endl;
    return -1;
  }
  int alpha_num_params = alpha_num_intervals + 1;
  float alpha_interval_size = (alphamax - alphamin) / (alpha_num_intervals);

  float betamin = 0;
  float betamax = 0.5;
  int beta_num_intervals = 200;
  if (!(std::istringstream(argv[3]) >> beta_num_intervals) ||
      !(beta_num_intervals > 0)) {
    std::cerr << "Number of beta-intervals must be positive Integer!"
              << std::endl;
    return -1;
  }
  int beta_num_params = beta_num_intervals + 1;
  float beta_interval_size = (betamax - betamin) / (beta_num_intervals);

  // fill Parametervectors:
  aligned_vector<float> alphas(alpha_num_params);
  float* alphasp = alphas.data();
#pragma omp simd aligned(alphasp : 64)
  for (int i = 0; i < alpha_num_params; i++) {
    alphasp[i] = alphamin + i * alpha_interval_size;
  }
  aligned_vector<float> betas(beta_num_params);
  float* betasp = betas.data();
#pragma omp simd aligned(betasp : 64)
  for (int i = 0; i < beta_num_params; i++) {
    betasp[i] = betamin + i * beta_interval_size;
  }

  aligned_vector<float> x_start = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
  // aligned_vector<float> y_start = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
  aligned_vector<float> y_start = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  // Output pixel vector into PGM File
  std::string filename = "picture.pgm";
  std::ofstream ostrm(filename);
  ostrm << "P2" << '\n';
  ostrm << alpha_num_params << ' ' << beta_num_params << '\n';
  ostrm << 255 << '\n';  // max. gray value
  aligned_vector<int> buffer(alpha_num_params * beta_num_params);

  auto time_start = std::chrono::system_clock::now();

#pragma omp parallel for
  for (int a = 0; a < alpha_num_params; a++) {
    for (int b = beta_num_params - 1; b >= 0; b--) {
      buffer[(beta_num_params - b - 1) * alpha_num_params + a] =
          pixel(alphas[a], betas[b], x_start, y_start, num_iterations);
    }
  }
  auto time_end = std::chrono::system_clock::now();
  float const elapsed_seconds =
      std::chrono::duration<float>(time_end - time_start).count();
  std::cout << "TIME: " << elapsed_seconds << std::endl;

  for (int i = 0; i < buffer.size(); ++i) {
    ostrm << buffer[i] << ' ';
    if (i % alpha_num_params == 0) {
      ostrm << '\n';
    }
  }
  return 0;
}

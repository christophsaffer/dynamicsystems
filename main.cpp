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
#include <boost/program_options.hpp>

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
  // these can be input by user
  int num_iterations;
  float alphamin;
  float alphamax;
  int alpha_num_intervals;
  float betamin;
  float betamax;
  int beta_num_intervals;

  namespace po = boost::program_options;
  try {
    po::options_description desc("Options");
    desc.add_options()
      ("help", "Help message")
      ("iterations,n", po::value<int>(&num_iterations)->default_value(100), "Number of iterations")
      ("amin,a", po::value<float>(&alphamin)->default_value(0), " α lower bound")
      ("amax,A", po::value<float>(&alphamax)->default_value(1), " α upper bound")
      ("alphas,w", po::value<int>(&alpha_num_intervals)->default_value(100), "α resolution/width of image")
      ("betas,h", po::value<int>(&beta_num_intervals)->default_value(100), "β resolution/height of image")
      ("bmin,b", po::value<float>(&betamin)->default_value(0), " β lower bound")
      ("bmax,B", po::value<float>(&betamax)->default_value(1), " β upper bound")
      ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;
    }

    po::notify(vm);

    // check if our integers are >0, else throw invalid-argument-error
    if ( (num_iterations < 1) || (alpha_num_intervals < 1) || (beta_num_intervals <1) ) {
      throw po::validation_error(po::validation_error::invalid_option_value);
    }

  } catch (po::error &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  // these are computed
  int alpha_num_params = alpha_num_intervals + 1;
  float alpha_interval_size = (alphamax - alphamin) / (alpha_num_intervals);
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

#pragma omp parallel for schedule(dynamic)
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

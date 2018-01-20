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

#include "colormaps.hpp"

template <typename T>
using aligned_allocator = boost::alignment::aligned_allocator<T, 64>;
template <typename T>
using aligned_vector = std::vector<T, aligned_allocator<T>>;

constexpr float PI = 3.141592635;
constexpr float TWO_PI = 6.28318530718;  // 2 * PI;

// return gray-value of pixel[alpha,beta], between 0..255
float pixel(float alpha, float beta, aligned_vector<float> seed_x,
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

  return d;
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

  aligned_vector<float> buffer(alpha_num_params * beta_num_params);
  aligned_vector<int> colors(alpha_num_params * beta_num_params);
  aligned_vector<int> colors_rgb(3 * alpha_num_params * beta_num_params);

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

  // transform floats into grayscale-colors:
  for (int i = 0; i < alpha_num_params*beta_num_params; ++i) {
    if (buffer[i] > 1) {
      colors[i] = 255;
      colors_rgb[3*i] = 255;
      colors_rgb[3*i+1] = 255;
      colors_rgb[3*i+2] = 255;
    } else {
      colors[i] = (int) 254 * buffer[i];
      // RGB color gradient: viridis from matplotlib
      int idx = floor(buffer[i]*255);
      
      int r = floor(255*viridis[idx][0]);
      int g = floor(255*viridis[idx][1]);
      int b = floor(255*viridis[idx][2]);

      colors_rgb[3*i] = r;  // red
      colors_rgb[3*i+1] = g;// green
      colors_rgb[3*i+2] = b;  // blue
    }
  }

  // Output result into .csv
  std::string file_result = "result.csv";
  std::ofstream ostrm_csv(file_result);
  ostrm_csv << "alpha beta value\n";
  for (int a = 0; a < alpha_num_params; a++) {
    for (int b = beta_num_params - 1; b >= 0; b--) {
      ostrm_csv << alphas[a] << ' ' << betas[b] << ' ' << buffer[(beta_num_params - b - 1) * alpha_num_params + a] << std::endl;
    }
  }

  // Output pixel vector into PGM File
  std::string filename = "picture.pgm";
  std::ofstream ostrm(filename);
  ostrm << "P5" << '\n';
  ostrm << alpha_num_params << ' ' << beta_num_params << '\n';
  ostrm << 255 << '\n';  // max. gray value
  // 1 byte per pixel
  for (int i=0;i<colors.size();++i){
  ostrm.write(reinterpret_cast<char*>(&colors[i]),1);//colors.size());
  }
  // Output pixel vector into PPM File
  std::string filename_rgb = "picture_rgb.ppm";
  std::ofstream ostrm_rgb(filename_rgb);
  ostrm_rgb << "P6" << '\n';
  ostrm_rgb << alpha_num_params << ' ' << beta_num_params << '\n';
  ostrm_rgb << 255 << '\n';  // max. gray value
  // 3 byte per pixel
  for (int i=0;i<colors_rgb.size();++i){
    // das geht theoretisch auch mit dem ganzen Array, aber praktisch nicht?!
  ostrm_rgb.write(reinterpret_cast<char*>(&colors_rgb[i]),1);//colors_rgb.size());
  }
  return 0;
}

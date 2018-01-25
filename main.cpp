#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

#include <boost/program_options.hpp>

#include <png.h>

#include "colormaps.hpp"
#include "compute.hpp"
#include "picture.hpp"

int main(int argc, char* argv[]) {
  // get arguments from CLI
  // these can be input by user
  int num_iterations;
  float threshold;
  float alphamin;
  float alphamax;
  int alpha_num_intervals;
  float betamin;
  float betamax;
  int beta_num_intervals;
  int num_seedpoints;
  bool output;
  std::vector<float> seedpoints;

  namespace po = boost::program_options;
  try {
    po::options_description desc("Options");
    desc.add_options()
      ("help", "Help message")
      ("iterations,n", po::value<int>(&num_iterations)->default_value(100),
      " Number of iterations")
      ("width,w", po::value<int>(&alpha_num_intervals)->default_value(100),
      " width of image (alpha resolution)")
      ("height,h", po::value<int>(&beta_num_intervals)->default_value(100),
      " height of image (beta resolution)")
      ("threshold,s", po::value<float>(&threshold)->default_value(1), 
      " Threshold above that computation is stopped")
      ("amin,a", po::value<float>(&alphamin)->default_value(0),
      " alpha lower bound")
      ("amax,A", po::value<float>(&alphamax)->default_value(1),
      " alpha upper bound")
      ("bmin,b", po::value<float>(&betamin)->default_value(0),
      " beta lower bound")
      ("bmax,B", po::value<float>(&betamax)->default_value(1),
      " beta upper bound")
      ("num_seedpoints,N", po::value<int>(&num_seedpoints)->default_value(8),
      " Number of seedpoints (uniformly distributed in (0,1) )")
      ("seedpoints,S", po::value<std::vector<float>>(&seedpoints)->multitoken(),
      " Values for explicit seedpoints")
      ("outputcsv,O", po::value<bool>(&output)->default_value(false),
      " Boolean flag for output a csv file")
      ;
      

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;
    }

    po::notify(vm);

    // check if our integers are >0, else throw invalid-argument-error
    if ((num_iterations < 1) || (alpha_num_intervals < 1) ||
        (beta_num_intervals < 1)) {
      throw po::validation_error(po::validation_error::invalid_option_value);
    }

  } catch (po::error& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }


  // these are computed
  int alpha_num_params = alpha_num_intervals + 1;
  float alpha_interval_size = (alphamax - alphamin) / (alpha_num_intervals);
  int beta_num_params = beta_num_intervals + 1;
  float beta_interval_size = (betamax - betamin) / (beta_num_intervals);

  // fill prametervectors alpha and beta
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

  // Initialization and output of seedpoints 
  std::cout << "Following seedpoints are used for computation:" << std::endl;
  aligned_vector<float> x_start(num_seedpoints);
  aligned_vector<float> y_start(num_seedpoints);
  for (int i = 1; i < num_seedpoints - seedpoints.size() + 1; ++i) {
    x_start[i - 1] = 0.5f * static_cast<float>(i) / (num_seedpoints - seedpoints.size() + 1);
    y_start[i - 1] = 0;
    std::cout << x_start[i - 1] << ", ";
  }
  for (int i = 0; i < seedpoints.size(); ++i) {
    x_start[num_seedpoints - seedpoints.size() + i] = seedpoints[i];
    y_start[num_seedpoints - seedpoints.size() + i] = 0;
    std::cout << x_start[num_seedpoints - seedpoints.size() + i] << ", ";
  }
  std::cout << '\n';

  // Initialization pixel values and color vectors
  aligned_vector<float> result(alpha_num_params * beta_num_params);

  auto time_start = std::chrono::system_clock::now();

  // Computation 
#pragma omp parallel for schedule(dynamic)
  for (int b = beta_num_params - 1; b >= 0; b--) {
    for (int a = 0; a < alpha_num_params; a++) {
      result[(beta_num_params - b - 1) * alpha_num_params + a] = compute(
          alphas[a], betas[b], x_start, y_start, num_iterations, threshold);
    }
  }
  auto time_end = std::chrono::system_clock::now();
  float elapsed_seconds =
      std::chrono::duration<float>(time_end - time_start).count();
  std::cout << "TIME for computation: " << elapsed_seconds << std::endl;


  time_start = std::chrono::system_clock::now();
  write_png("picture.png", result.data(), threshold, alpha_num_params, beta_num_params);
  time_end = std::chrono::system_clock::now();
  elapsed_seconds =  std::chrono::duration<float>(time_end - time_start).count();
  std::cout << "TIME for picture: " << elapsed_seconds << std::endl;

    // Generate output
  if (output) {
    time_start = std::chrono::system_clock::now();
   // Output result into .csv
    std::string file_result = "result.csv";
    std::ofstream ostrm_csv(file_result);
    ostrm_csv << "alpha beta value\n";
    for (int b = beta_num_params - 1; b >= 0; b--) {
      for (int a = 0; a < alpha_num_params; a++) {
        ostrm_csv << alphas[a] << ' ' << betas[b] << ' '
                  << result[(beta_num_params - b - 1) * alpha_num_params + a]
                  << '\n';
      }
    }
  time_end = std::chrono::system_clock::now();
  elapsed_seconds =  std::chrono::duration<float>(time_end - time_start).count();
  std::cout << "TIME for csv: " << elapsed_seconds << std::endl;
  }
}

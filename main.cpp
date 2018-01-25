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
    desc.add_options()("help", "Help message")(
        "iterations,n", po::value<int>(&num_iterations)->default_value(100),
        " Number of iterations")(
        "threshold,s", po::value<float>(&threshold)->default_value(1),
        " Threshold above that computation is stopped")(
        "amin,a", po::value<float>(&alphamin)->default_value(0),
        " α lower bound")("amax,A",
                          po::value<float>(&alphamax)->default_value(1),
                          " α upper bound")(
        "alphas,w", po::value<int>(&alpha_num_intervals)->default_value(100),
        " α resolution/width of image")(
        "betas,h", po::value<int>(&beta_num_intervals)->default_value(100),
        " β resolution/height of image")(
        "bmin,b", po::value<float>(&betamin)->default_value(0),
        " β lower bound")("bmax,B",
                          po::value<float>(&betamax)->default_value(1),
                          " β upper bound")(
        "num_seedpoints,N", po::value<int>(&num_seedpoints)->default_value(8),
        " Number of seedpoints (uniformly distributed in (0,1) )")(
        "seedpoints,S", po::value<std::vector<float>>(&seedpoints)->multitoken(), 
        " Values for explicit seedpoints")(
        "output,O", po::value<bool>(&output)->default_value(true),
        " Boolean flag for output");
      

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
  aligned_vector<unsigned char> colors(alpha_num_params * beta_num_params);
  aligned_vector<unsigned char> colors_rgb(3 * alpha_num_params * beta_num_params);

  auto time_start = std::chrono::system_clock::now();

  // Computation 
#pragma omp parallel for schedule(dynamic)
  for (int a = 0; a < alpha_num_params; a++) {
    for (int b = beta_num_params - 1; b >= 0; b--) {
      result[(beta_num_params - b - 1) * alpha_num_params + a] = compute(
          alphas[a], betas[b], x_start, y_start, num_iterations, threshold);
    }
  }
  auto time_end = std::chrono::system_clock::now();
  float const elapsed_seconds =
      std::chrono::duration<float>(time_end - time_start).count();
  std::cout << "TIME: " << elapsed_seconds << std::endl;

  // Generate output
  if (output) {
    // Output result into .csv
    std::string file_result = "result.csv";
    std::ofstream ostrm_csv(file_result);
    ostrm_csv << "alpha beta value\n";
    for (int a = 0; a < alpha_num_params; a++) {
      for (int b = beta_num_params - 1; b >= 0; b--) {
        ostrm_csv << alphas[a] << ' ' << betas[b] << ' '
                  << result[(beta_num_params - b - 1) * alpha_num_params + a]
                  << std::endl;
      }
    }

    // transform floats into grayscale-colors:
    for (int i = 0; i < alpha_num_params * beta_num_params; ++i) {
      if (result[i] > threshold) {
        colors[i] = 255;
        colors_rgb[3 * i] = 255;
        colors_rgb[3 * i + 1] = 255;
        colors_rgb[3 * i + 2] = 255;
      } else {
        colors[i] = floor(254 * result[i] / threshold);
        // RGB color gradient: viridis from matplotlib
        int idx = floor(255 * result[i] / threshold);

        unsigned char r = floor(255 * viridis[idx][0]);
        unsigned char g = floor(255 * viridis[idx][1]);
        unsigned char b = floor(255 * viridis[idx][2]);

        colors_rgb[3 * i] = r;      // red
        colors_rgb[3 * i + 1] = g;  // green
        colors_rgb[3 * i + 2] = b;  // blue
      }
    }

    // Output pixel vector into PGM File
    std::ofstream ostrm("picture.pgm", std::ofstream::binary);
    ostrm << "P5" << '\n';
    ostrm << alpha_num_params << ' ' << beta_num_params << '\n';
    ostrm << 255 << '\n';  // max. gray value
    // 1 byte per pixel
    for (int i = 0; i < colors.size(); ++i) {
      ostrm << static_cast<char>(colors[i]);  // colors.size());
    }
    // Output pixel vector into PPM File
    std::ofstream ostrm_rgb("picture_rgb.ppm", std::ofstream::binary);
    ostrm_rgb << "P6" << '\n';
    ostrm_rgb << alpha_num_params << ' ' << beta_num_params << '\n';
    ostrm_rgb << 255 << '\n';  // max. gray value
    // 3 byte per pixel
    for (int i = 0; i < colors_rgb.size(); ++i) {
      // das geht theoretisch auch mit dem ganzen Array, aber praktisch nicht?!
      ostrm_rgb << static_cast<char>(colors_rgb[i]);
    }

    // write to libpng
      png_image img;
      memset(&img, 0, sizeof(img));
      img.version = PNG_IMAGE_VERSION;
      img.opaque = NULL;
      img.width = alpha_num_params;
      img.height = beta_num_params;
      img.format = PNG_FORMAT_RGB;
      img.flags = 0;
      img.colormap_entries = 0;

      // set to negative for bottom-up image
      const int row_stride = alpha_num_params * 3;

      png_bytep buffer = colors_rgb.data();

      png_image_write_to_file(&img, "picture_rgb.png", false, buffer, row_stride, NULL);
      if (PNG_IMAGE_FAILED(img)) {
        std::cerr << img.message << std::endl;
        return -1;
      } else {
        if (img.warning_or_error != 0) {
          std::cerr << img.message << std::endl;
        }
      }
      std::cout << "png written" << std::endl;
  }
}

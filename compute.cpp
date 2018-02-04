#include "compute.hpp"

#include <algorithm>

float compute(float alpha, float beta, aligned_vector<float> seed_x,
              aligned_vector<float> seed_y, int num_iterations,
              float threshold) {
  int num_seeds = seed_x.size();
  assert(seed_y.size() == num_seeds);

  aligned_vector<float> x = seed_x;
  aligned_vector<float> y = seed_y;

  float* xp = x.data();
  float* yp = y.data();

  float d = 0.0;

  for (int i = 0; i < num_iterations && d < threshold; i++) {
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
  ) {

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
  if (output_csv) {
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


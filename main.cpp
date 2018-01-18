#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <chrono>

static const double PI = std::acos(-1);
static const double TWO_PI = 2 * PI;

// return gray-value of pixel[alpha,beta], between 0..255
int pixel(double alpha, double beta, std::vector<double> seed_x, std::vector<double> seed_y, int num_iterations) {
  int num_seeds = seed_x.size();
  assert(seed_y.size() == num_seeds);
  
  std::vector<double> x = seed_x;
  std::vector<double> y = seed_y;
  
  double d = 0.0;

  for (int i=0;i<num_iterations && d<1;i++) {
    for (int s=0;s<num_seeds;s++) {
      y[s] = y[s] + beta * std::sin(TWO_PI * x[s]);
    }
    for (int s=0; s<num_seeds;s++){
      x[s] = x[s] + alpha * std::sin(TWO_PI * y[s]);
    }

    for (int s=0;s<num_seeds;s++) {
      d = std::max(d,std::abs(y[s]));
    }
  }
  
  if (d>1) { 
    return 255;
  } else {
    return (int) 254*d;
  }
}

int main(int argc, char* argv[]) {

  // get arguments from CLI
  if ( argc != 4) {
    std::cerr << "usage: " << argv[0] << " <num_iterations> <num_alpha_intervals> <num_beta_intervals>" << std::endl;
    return -1;
  }
  // Default parameters
  int num_iterations = 100;
  if (!(std::istringstream(argv[1]) >> num_iterations) || !(num_iterations > 0) ) {
    std::cerr << "Number of Iterations must be positive Integer!" << std::endl;
    return -1;
  }

  double alphamin = 0;
  double alphamax = 2;
  int alpha_num_intervals = 600;
  if (!(std::istringstream(argv[2]) >> alpha_num_intervals) || !(alpha_num_intervals > 0) ) {
    std::cerr << "Number of alpha-intervals must be positive Integer!" << std::endl;
    return -1;
  }
  int alpha_num_params = alpha_num_intervals + 1;
  double alpha_interval_size = (alphamax-alphamin)/(alpha_num_intervals);

  double betamin = 0;
  double betamax = 0.5;
  int beta_num_intervals = 200;
  if (!(std::istringstream(argv[3]) >> beta_num_intervals) || !(beta_num_intervals > 0) ) {
    std::cerr << "Number of beta-intervals must be positive Integer!" << std::endl;
    return -1;
  }
  int beta_num_params = beta_num_intervals + 1;
  double beta_interval_size = (betamax-betamin)/(beta_num_intervals);


  // fill Parametervectors:
  std::vector<double> alphas(alpha_num_params);
  for (int i=0;i<alpha_num_params;i++) {
    alphas[i] = alphamin + i*alpha_interval_size;
  }
  std::vector<double> betas(beta_num_params);
  for (int i=0;i<beta_num_params;i++) {
    betas[i] = betamin + i*beta_interval_size;
  }

  std::vector<double> x_start = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
  //std::vector<double> y_start = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
  std::vector<double> y_start = {0,0,0,0,0,0,0,0,0};

  // Output pixel vector into PGM File
  std::string filename = "picture.pgm";
  std::ofstream ostrm(filename);
  ostrm << "P2" << '\n';
  ostrm << alpha_num_params << ' ' << beta_num_params << '\n';
  ostrm << 255 << '\n'; // max. gray value
  std::vector<int> buffer(alpha_num_params*beta_num_params);
  
  auto time_start = std::chrono::system_clock::now();

#pragma omp parallel for
  for (int b=beta_num_params-1;b>=0;b--) {
    for (int a=0;a<alpha_num_params;a++) {
     buffer[(beta_num_params-b-1)*alpha_num_params+a] = pixel( alphas[a],betas[b], x_start, y_start, num_iterations); 
    }
  }
  auto time_end = std::chrono::system_clock::now();
  double const elapsed_seconds = std::chrono::duration<double>(time_end - time_start).count();
  std::cout << "TIME: " << elapsed_seconds << std::endl;

  for(int i=0;i<buffer.size();++i) {
    ostrm << buffer[i] << ' ';
    if(i%alpha_num_params==0) {
      ostrm << '\n';
    }
  }
  return 0;
}

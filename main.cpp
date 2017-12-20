#include <vector>
#include <random>
#include <cmath>
#include <iostream>

static const double PI = std::acos(-1);

int main(int argc, char* argv[]) {

  // Default parameters
  int iterations = 100;
  int seedpoints = 10;
  int intervals = 100;

  // Get parameters from command line
  if (argc == 1) {
    std::cout << "Warning: Not all parameters were given. Using parameters " 
    << iterations << ", " << seedpoints << ", " << intervals << " .." <<std::endl;
  }
  else if (argc == 2) {
    iterations = atoi(argv[1]);
    std::cout << "Warning: Not all parameters were given. Using parameters " 
    << iterations << ", " << seedpoints << ", " << intervals << " .." <<std::endl;
  }
  else if (argc == 3) {
    iterations = atoi(argv[1]);
    seedpoints = atoi(argv[2]);
    std::cout << "Warning: Not all parameters were given. Using parameters " 
    << iterations << ", " << seedpoints << ", " << intervals << " .." <<std::endl;
  }
  else {
    iterations = atoi(argv[1]);
    seedpoints = atoi(argv[2]);
    intervals = atoi(argv[3]);
  }

  double alphabetamax = 1;
  double intervalsize = alphabetamax/intervals;
  std::cout << "intervallsize: " << intervalsize <<std::endl;

  // Generate pixel vectors
  const int sample_size = seedpoints * intervals * intervals;
  std::vector<double> x(sample_size);
  std::vector<double> y(sample_size);
  double * x_ = x.data();
  double * y_ = y.data();


  // Generate seedpoints
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<> dis(0, 1);
  std::vector<double> randv;
  for(int i=0; i < seedpoints; ++i) {
    double randn = dis(gen);
    randv.push_back(randn);
  }

  // Fill pixel vectors with seedpoints
  int j; 
  for (int i = 0; i < sample_size; ++i) {
    j = i % seedpoints;
    y_[i] = randv[j];
    x_[i] = randv[j];
  }


  const double TWO_PI = 2 * PI;
  for (int j = 0; j < iterations; ++j) {
    for (int i = 0; i < sample_size; ++i) {
      double beta = ((i / seedpoints) % intervals) * intervalsize;
      double alpha = int((i / seedpoints) / intervals) * intervalsize;
      // std::cout << i << ": beta: " << beta << " -- alpha: " << alpha <<std::endl;
      y_[i] = y_[i] + beta * std::sin(TWO_PI * x_[i]);
      x_[i] = x_[i] + alpha * std::sin(TWO_PI * y_[i]);
    }
  }

  for (int i = 0; i < sample_size; ++i) {
    std::cout << i << " - y: " << y_[i] << ", x: " << x_[i] <<std::endl;
  }




  return 0;
}

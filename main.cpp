#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <iostream>
#include <fstream>
#include <cassert>

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

  // Default parameters
  int num_iterations = 100;
  int num_intervals = 500;
  int num_params = num_intervals + 1;

  double alphabetamin = 0;
  double alphabetamax = 2;
  double intervalsize = (alphabetamax-alphabetamin)/(num_intervals);
  std::cout << "intervallsize: " << intervalsize <<std::endl;

  // fill Parametervectors:
  std::vector<double> params(num_params);
  for (int i=0;i<num_params;i++) {
    params[i] = alphabetamin + i*intervalsize;
  }

  std::vector<double> x_start = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
  //std::vector<double> y_start = {0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
  std::vector<double> y_start = {0,0,0,0,0,0,0,0,0};

  // Output pixel vector into PGM File
  std::string filename = "picture.pgm";
  std::ofstream ostrm(filename);
  ostrm << "P2" << '\n';
  ostrm << num_params << ' ' << num_params << '\n';
  ostrm << 255 << '\n'; // max. gray value
  std::vector<int> buffer(num_params*num_params);
  
#pragma omp parallel for
  for (int j=num_params-1;j>=0;j--) {
    for (int i=0;i<num_params;i++) {
     buffer[j*num_params+i] = pixel( params[i],params[j], x_start, y_start, num_iterations); 
    }
  }

  for(int i=0;i<buffer.size();++i) {
    ostrm << buffer[i] << ' ';
    if(i%num_params==0) {
      ostrm << '\n';
    }
  }
  return 0;
}

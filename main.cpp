#include <vector>
#include <random>
#include <cmath>
#include <iostream>

static const double PI = std::acos(-1);

double f(double alpha, double x, double y) {
  return x + alpha*sin(2*PI*y);
}

double g(double beta, double x, double y) {
  return y + beta*sin(2*PI*x);
}

bool cond(double x, double y, double seedx, double seedy) {
  return true;
}

int main(int argc, char** argv) {
  int n = 10;  // number of iterations
  int m = 100; // number of seed points
  int alphamax = 1;
  //int betamax = 1;
  int steps = 2000;
  
  double stepsize = alphamax/steps;

  const int sample_size = m * steps * steps;
  std::vector<double> x(sample_size);
  std::vector<double> y(sample_size);

  double * x_ = x.data();
  double * y_ = y.data();


  // Generating vector of random numbers
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<> dis(0, 1);
  std::vector<double> randv;
  for(int i=0; i < m; ++i) {
    double randn = dis(gen);
    randv.push_back(randn);
  }

  //std::unique_ptr<double[]> tmp{new double[sample_size]};


    //std::vector<double> beta_vec(sample_size);
    //for (int i = 0; i < sample_size; ++i) beta_vec[i] = (i % m) * stepsize; 
    const double TWO_PI = 2 * PI;
    for (int i = 0; i < sample_size; ++i) {
      const double beta = (i % m) * stepsize;
      const double alpha = (i % m) * stepsize;
      //const double alpha = (i % (m * steps)) * stepsize;
      y_[i] = y_[i] + beta * std::sin(TWO_PI * x_[i]);
      x_[i] = x_[i] + alpha * std::sin(TWO_PI * y_[i]);
    }
  
    //for (int i = 0; i < sample_size; ++i) {
      
      //const double alpha = (i % (m * steps)) * stepsize;
     // x_[i] = x_[i] + alpha * std::sin(TWO_PI * y_[i]);
    //}

  // Filling the vectors with inital random values


  // Iterating over all pixel in the grid
  // Adressing: (i * stepsize), (j * stepsize)
  // double alpha;
  // double beta;
  // for (int i=0; i < steps; ++i) {
  //   alpha = i * stepsize
  //   for (int j=0; j < steps; ++j) {
  //     beta = j * stepsize
  //     for (int k=0; k < m; ++k) {
  //       x[i + j + m]
  //     }
  //   }
  // }

  return 0;
}

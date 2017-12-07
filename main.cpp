#include <vector>
#include <random>
#include <cmath>

const double pi = std::acos(-1);

double f(double alpha, double x, double y) {
  return x + alpha*sin(2*pi*y);
}

double g(double beta, double x, double y) {
  return y + beta*sin(2*pi*x);
}

bool cond(double x, double y, double seedx, double seedy) {
  return true;
}

int main(int argc, char** argv) {
  int n = 1000;  // number of iterations
  int m = 10; // number of seed points
  int alphamax = 1;
  int betamax = 1;
  int steps = 1000;
  
  double stepsize = alphamax/steps;

  std::vector<double> x(m*steps*steps);
  std::vector<double> y(m*steps*steps);

  // Generating vector of random numbers
  std::mt19937 gen(std::random_device{}());
  std::uniform_real_distribution<> dis(0, 1);
  std::vector<double> randv = {};
  for(int i=0; i < m; ++i) {
    double randn = dis(gen);
    randv.push_back(randn);
  }

  // Iterating over all pixel in the grid
  // Adressing: (i * stepsize) + j + randv(k)
  for (int i=0; i < steps; ++i) {
    for (int j=0; j < steps; ++j) {
      for (int k=0; k < m; ++k) {

      }
    }
  }




  return 0;
}

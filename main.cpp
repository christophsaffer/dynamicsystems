#include <vector>
#include <cmath>

const double pi = std::acos(-1);

double f(double alpha, double x, double y) {
  return x + alpha*sin(2*pi*y);
}
double g(double beta, double x, double y) {
  return y + beta*sin(2*pi*x);
}

int main(int argc, char** argv) {
  int n = 1000;  // number of iterations
  int m = 10; // number of starting points
  int alphamax = 1;
  int betamax = 1;
  int steps = 1000;
  
  double stepsize = alphamax/steps;

  std::vector<double> x(n*m*steps);
  std::vector<double> y(n*m*steps);

  return 0;
}

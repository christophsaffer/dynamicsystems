#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

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
  bool output_csv;
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
      ("threshold,t", po::value<float>(&threshold)->default_value(1), 
      " Threshold above that computation is stopped")
      ("amin,a", po::value<float>(&alphamin)->default_value(0),
      " alpha lower bound")
      ("amax,A", po::value<float>(&alphamax)->default_value(1),
      " alpha upper bound")
      ("bmin,b", po::value<float>(&betamin)->default_value(0),
      " beta lower bound")
      ("bmax,B", po::value<float>(&betamax)->default_value(1),
      " beta upper bound")
      ("num_seedpoints,m", po::value<int>(&num_seedpoints)->default_value(8),
      " Number of seedpoints (uniformly distributed in (0,1) )")
      ("seedpoints,S", po::value<std::vector<float>>(&seedpoints)->multitoken(),
      " Values for explicit seedpoints")
      ("csv,O", po::value<bool>(&output_csv)->default_value(false),
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

  compute_all(
    num_iterations,
    threshold,
    alphamin,
    alphamax,
    alpha_num_intervals,
    betamin,
    betamax,
    beta_num_intervals,
    num_seedpoints,
    output_csv,
    seedpoints
  );

}

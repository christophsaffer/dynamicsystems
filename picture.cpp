#include "picture.hpp"

#include <cmath>
#include <csetjmp>
#include <cstdio>

#include <iostream>
#include <stdexcept>
#include <vector>

#include <png.h>

#include "colormaps.hpp"

// all functions for picture transformation and output:

class FileWrapper {
  FILE *file_;

 public:
  FileWrapper(const std::string &filename, const char *open_mode)
      : file_(std::fopen(filename.c_str(), open_mode)) {
    if (!file_) throw std::runtime_error("error opening file " + filename);
  }

  ~FileWrapper() {
    if (file_) {
      fclose(file_);
      file_ = nullptr;
    }
  }

  operator FILE *() { return file_; }
};

// function: map result to color vector
bool write_png(const char *filename, const float *result, float threshold,
               int width, int height) {
  std::vector<unsigned char> colors_gray(width * height);
  std::vector<unsigned char> colors_rgb(3 * width * height);

  for (int i = 0; i < width * height; ++i) {
    if (result[i] > threshold) {
      colors_gray[i] = 255;
      colors_rgb[3 * i] = 255;
      colors_rgb[3 * i + 1] = 255;
      colors_rgb[3 * i + 2] = 255;
    } else {
      colors_gray[i] = std::floor(254 * result[i] / threshold);
      // RGB color gradient: viridis from matplotlib
      int idx = std::floor(255 * result[i] / threshold);

      unsigned char r = std::floor(255 * viridis[idx][0]);
      unsigned char g = std::floor(255 * viridis[idx][1]);
      unsigned char b = std::floor(255 * viridis[idx][2]);

      colors_rgb[3 * i] = r;      // red
      colors_rgb[3 * i + 1] = g;  // green
      colors_rgb[3 * i + 2] = b;  // blue
    }
  }

  FileWrapper file(filename, "wb");
  png_structp png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) return false;

  png_infop info = png_create_info_struct(png);
  if (!info) return false;

  if (std::setjmp(png_jmpbuf(png))) return false;

  png_init_io(png, file);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png, info);

  std::vector<png_bytep> row_pointers(height);
  const int row_stride = width * 3;
  for (int i = 0; i < row_pointers.size(); ++i)
    row_pointers[i] = colors_rgb.data() + i * row_stride;

  png_write_image(png, row_pointers.data());
  png_write_end(png, NULL);
  return true;
}

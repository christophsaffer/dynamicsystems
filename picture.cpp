#include "picture.hpp"

// all functions for picture transformation and output:

// function: map result to color vector
bool write_png(const char * filename, const float *result, float threshold, int width, int height) {
  
  std::vector<unsigned char> colors_gray(width * height);
  std::vector<unsigned char> colors_rgb(3 * width * height);

  for (int i = 0; i < width * height; ++i) {
    if (result[i] > threshold) {
      colors_gray[i] = 255;
      colors_rgb[3 * i] = 255;
      colors_rgb[3 * i + 1] = 255;
      colors_rgb[3 * i + 2] = 255;
    } else {
      colors_gray[i] = floor(254 * result[i] / threshold);
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
  png_bytep buffer = colors_rgb.data();

  png_image img;
  memset(&img, 0, sizeof(img));
  img.version = PNG_IMAGE_VERSION;
  img.opaque = NULL;
  img.width = width;
  img.height = height;
  img.format = PNG_FORMAT_RGB;
  img.flags = 0;
  img.colormap_entries = 0;

  // set to negative for bottom-up image
  const int row_stride = width * 3;

  png_image_write_to_file(&img, filename, false, buffer, row_stride, NULL);
  if (PNG_IMAGE_FAILED(img)) {
    std::cerr << img.message << std::endl;
    return false;
  } else {
    if (img.warning_or_error != 0) {
      std::cerr << img.message << std::endl;
    }
    return true;
  }
}

#ifndef PICTURE_H
#define PICTURE_H

#include <boost/align/aligned_allocator.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <cstring>
#include <png.h>
#include "colormaps.hpp"

bool write_png(const char * filename, const float *result, float threshold, int width, int height);

#endif

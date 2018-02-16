#ifndef PICTURE_H
#define PICTURE_H

bool write_png(const char *filename, const float *result, float threshold,
               int width, int height);

#endif

#ifndef UTILS_H
#define UTILS_H
#include "geometry.hpp"
#include "model.h"
#include "ppm.hpp"

namespace pd {

void render_man(int width, int height, float *zbuffer, PPMImage &image,
                const PPMImage *texture = nullptr);

void render_random_triangle(int width, int height, float *zbuffer,
                            PPMImage &image);

void render_triangle(int width, int height, float *zbuffer, PPMImage &image);

void render_quad(float *zbuffer, PPMImage &image, const PPMImage *texture);

void render_cube(float *zbuffer, PPMImage &image, const PPMImage *texture,
                 bool phong = false);

}; // namespace pd
#endif

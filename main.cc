#include "geometry.hpp"
#include "model.h"
#include "ppm.hpp"
#include "utils.h"
#include <camera.h>
#include <memory>
using namespace pd;
#include <cstring>
#include <random>
int main(int argc, char *argv[]) {
  std::string f("file.ppm");
  constexpr int width = 800;
  constexpr int height = width;
  PPMImage image(width, height);
  Pixel tmp{100, 100, 100};
  std::fill(image.image_.begin(), image.image_.end(), tmp);
  auto zbuffer = std::unique_ptr<std::array<float, width * height>>(
      new std::array<float, width * height>());
  zbuffer->fill(std::numeric_limits<float>::lowest());

  PPMImage texture;
  ppm3_read("diffuse.ppm", &texture.width_, &texture.height_, &texture.image_);
  texture.horizontal_flip();
  render_quad(zbuffer->data(), image, &texture);
  ppm3_write(f.c_str(), image.width_, image.height_, image.image_);

  return 0;
}

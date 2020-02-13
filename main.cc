#include "geometry.hpp"
#include "ppm.hpp"
using namespace pd;
#include <cstring>
int main(int argc, char *argv[]) {
  std::string f("file.ppm");
  PPMImage image(800, 600);
  Pixel tmp{100, 100, 100};
  std::fill(image.image_.begin(), image.image_.end(), tmp);
  image.set_pixel(10, 10, {255, 255, 255});
  line(10, 10, 30, 30, image, PURPLE);
  std::array<glm::vec3, 3> points;
  points[0] = glm::vec3(100, 10, 0);
  points[1] = glm::vec3(550, 10, 0);
  points[2] = glm::vec3(500, 100, 0);
  triangle(points, image, YELLOW);
  ppm3_write(f.c_str(), image.width_, image.height_, image.image_);

  std::vector<Pixel> vec_2;
  int width;
  int height;
  ppm3_read(f.c_str(), &width, &height, &vec_2);

  ppm3_write("file2.ppm", width, height, vec_2);
  return 0;
}

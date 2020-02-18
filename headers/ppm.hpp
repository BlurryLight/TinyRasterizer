#pragma once
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
namespace pd {
struct Pixel {
  float r_ = 0;
  float g_ = 0;
  float b_ = 0;
  Pixel() {}
  Pixel(float r, float g, float b) {
    r_ = r;
    g_ = g;
    b_ = b;
  }
  Pixel(const glm::vec3 &vec) {
    r_ = vec.r;
    g_ = vec.g;
    b_ = vec.b;
  }
  operator glm::vec3() const { return glm::vec3(r_, g_, b_); }
};
using Color = struct Pixel;
#define BLACK                                                                  \
  { 0, 0, 0 }
#define WHITE                                                                  \
  { 255, 255, 255 }
#define RED                                                                    \
  { 255, 0, 0 }
#define BLUE                                                                   \
  { 0, 0, 255 }
#define YELLOW                                                                 \
  { 255, 255, 0 }
#define PURPLE                                                                 \
  { 255, 0, 255 }
#define GREEN                                                                  \
  { 0, 255, 0 }

struct PPMImage {
  int width_;
  int height_;
  std::vector<Pixel> image_;
  PPMImage() {
    width_ = 640;
    height_ = 480;
    image_ = std::vector<Pixel>(width_ * height_);
  }
  PPMImage(int width, int height) {
    width_ = width;
    height_ = height;
    image_ = std::vector<Pixel>(width_ * height_);
  }
  void horizontal_flip() {
    for (int row = 0; row < height_ / 2; row++) {
      for (int col = 0; col < width_; col++) {
        std::swap(image_[row * width_ + col],
                  image_[(height_ - 1 - row) * width_ + col]);
      }
    }
  }
  void set_pixel(int row, int col,
                 const Pixel &p) //(0,1) 0 row 1 col = 2nd pixel
  {
    if (row >= height_ || col >= width_) {
      std::cerr << "set_pixel index error" << std::endl;
      return;
    }
    image_[width_ * row + col] = p;
  }
  Pixel get_pixel(int row, int col) const {
    if (row > height_ || col > width_) {
      std::cerr << "get_pixel index error" << std::endl;
      return glm::vec3(0, 0, 0);
    }
    return image_[width_ * row + col];
  }
};

// write picture as ppm3 ASCII
inline int ppm3_write(const char *path, int width, int height,
                      const std::vector<Pixel> &vec) {
  std::ofstream ofs(path, std::ios::out);
  ofs << "P3\n" << width << " " << height << "\n255\n";
  for (auto pixel : vec) {
    ofs << int(pixel.r_) << ' ' << int(pixel.g_) << ' ' << int(pixel.b_) << ' '
        << '\n';
  }
  return 0;
}
// don't support comments
inline int ppm3_read(const char *path, int *width, int *height,
                     std::vector<Pixel> *vec) {
  // clean the vec
  if (!vec->empty()) {
    auto tmp = std::vector<Pixel>();
    vec->swap(tmp);
  }
  std::ifstream ifs(path, std::ios::in);
  std::string token;
  ifs >> token;
  if (ifs.fail()) {
    std::cerr << "cannot open " << path << std::endl;
    return -1;
  }
  if (token != "P3") {
    std::cerr << "unsupported ppm " << path << std::endl;
    return -1;
  }
  // width
  ifs >> token;
  *width = std::stoi(token, nullptr);
  // height
  ifs >> token;
  *height = std::stoi(token, nullptr);
  // 255
  ifs >> token;
  if (token != "255") {
    std::cerr << "unsupported ppm " << path << std::endl;
    return -1;
  }
  ifs.get(); // skip the \n in 255\n
  int count = 0;
  int buf[3];
  Pixel tmp;
  while (ifs >> token) {
    if (token == "\n")
      continue;
    buf[count] = std::stoi(token);
    count++;
    if (count == 3) {
      tmp.r_ = buf[0];
      tmp.g_ = buf[1];
      tmp.b_ = buf[2];
      vec->push_back(tmp);
      count = 0;
    }
  }
  return 0;
}

} // namespace pd

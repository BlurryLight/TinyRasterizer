#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
namespace pd {
struct Pixel {
  unsigned int r_ = 0;
  unsigned int g_ = 0;
  unsigned int b_ = 0;
  Pixel() {}
  Pixel(int r, int g, int b) {
    r_ = r;
    g_ = g;
    b_ = b;
  }
};
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
  void set_pixel(int row, int col,
                 const Pixel &p) //(0,1) 0 row 1 col = 2nd pixel
  {
    if (row >= height_ || col >= width_) {
      std::cerr << "set_pixel index error" << std::endl;
      return;
    }
    image_[width_ * row + col] = p;
  }
};

// write picture as ppm3 ASCII
inline int ppm3_write(const char *path, int width, int height,
                      const std::vector<Pixel> &vec) {
  std::ofstream ofs(path, std::ios::out);
  ofs << "P3\n" << width << " " << height << "\n255\n";
  for (auto pixel : vec) {
    ofs << pixel.r_ << ' ' << pixel.g_ << ' ' << pixel.b_ << ' ' << '\n';
  }
  return 0;
}
// don't support comments
inline int ppm3_read(const char *path, int *width, int *height,
                     std::vector<Pixel> *vec) {
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
  while (std::getline(ifs, token)) {
    std::istringstream line(token);
    Pixel tmp;
    line >> tmp.r_ >> tmp.g_ >> tmp.b_;
    vec->push_back(tmp);
  }
  return 0;
}

} // namespace pd

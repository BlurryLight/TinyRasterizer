#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
namespace pd {
struct Pixel {
  unsigned int r, g, b;
};

// write picture as ppm3 ASCII
inline int ppm3_write(const char *path, int width, int height,
                      const std::vector<Pixel> &vec) {
  std::ofstream ofs(path, std::ios::out);
  ofs << "P3\n" << width << " " << height << "\n255\n";
  for (auto pixel : vec) {
    ofs << pixel.r << ' ' << pixel.g << ' ' << pixel.b << ' ' << '\n';
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
    line >> tmp.r >> tmp.g >> tmp.b;
    vec->push_back(tmp);
  }
  return 0;
}

} // namespace pd

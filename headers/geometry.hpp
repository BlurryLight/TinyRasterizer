#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP
#include "model.h"
#include "ppm.hpp"
#include <array>
#include <glm/glm.hpp>
namespace pd {
inline void line(int x0, int y0, int x1, int y1, PPMImage &image, Color color) {
  bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
  if (steep) {
    std::swap(y0, x0);
    std::swap(y1, x1);
  }
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  int deltax = x1 - x0;
  int deltay = std::abs(y1 - y0);
  int derror = std::abs(deltay) * 2; // multiply dx * 2
  int error = 0;
  int y = y0;
  for (int x = x0; x < x1; x++) {
    steep ? image.set_pixel(y, x, color) : image.set_pixel(x, y, color);
    error = error + derror;
    if (error > deltax) {
      y += (y1 > y0 ? 1 : -1);
      error = error - 2 * deltax;
    }
  }
}

// A,B,C in triangles
// P is the Point
// https://www.cnblogs.com/graphics/archive/2010/08/05/1793393.html
inline bool point_in_triangle(glm::vec3 A, glm::vec3 B, glm::vec3 C,
                              glm::vec3 P) {
  glm::vec3 v0 = C - A;
  glm::vec3 v1 = B - A;
  glm::vec3 v2 = P - A;

  float v0dotv0 = glm::dot(v0, v0);
  float v0dotv1 = glm::dot(v0, v1);
  float v1dotv2 = glm::dot(v2, v1);
  float v0dotv2 = glm::dot(v0, v2);
  float v1dotv1 = glm::dot(v1, v1);

  float denomimator = v0dotv0 * v1dotv1 - v0dotv1 * v0dotv1;
  if (denomimator == 0.0) {
    return false;
  }
  float u_numerator = v1dotv1 * v0dotv2 - v0dotv1 * v1dotv2;
  float v_numerator = v0dotv0 * v1dotv2 - v0dotv1 * v0dotv2;
  float u = u_numerator / denomimator;
  float v = v_numerator / denomimator;

  if (u < 0 || u > 1 || v < 0 || v > 1) {
    return false;
  }
  return u + v <= 1;
}
// plain triangle(fill the color)
void triangle(std::array<glm::vec3, 3> points, float *zbuffer, PPMImage &image,
              Color color) {
  glm::vec3 bbox_min{image.width_ - 1, image.height_ - 1, 0};
  glm::vec3 bbox_max{0, 0, 0};
  for (auto i : points) {
    bbox_max.x = std::max(i.x, bbox_max.x);
    bbox_max.y = std::max(i.y, bbox_max.y);
    bbox_min.x = std::min(i.x, bbox_min.x);
    bbox_min.y = std::min(i.y, bbox_min.y);
  }
  glm::vec3 normal =
      glm::normalize(glm::cross(points[2] - points[0], points[1] - points[0]));
  glm::vec3 p;
  //  (P - points[0]) * normal = 0;
  for (p.x = bbox_min.x; p.x < bbox_max.x; p.x++) {
    for (p.y = bbox_min.y; p.y < bbox_max.y; p.y++) {
      // point-normal formular(that's the math way)
      p.z =
          ((-normal.x * (p.x - points[0].x) - normal.y * (p.y - points[0].y)) /
           normal.z) +
          points[0].z;

      if (point_in_triangle(points[0], points[1], points[2], p)) {
        if (zbuffer[int(p.y * image.width_ + p.x)] <= p.z) {
          image.set_pixel(int(p.x), int(p.y), color);
          zbuffer[int(p.y * image.width_ + p.x)] = p.z;
        }
      }
    }
  }
}
// uv triangle
void triangle(std::array<Vertex, 3> points, float *zbuffer, PPMImage &image,
              std::array<glm::vec3, 3> colors) {
  glm::vec3 bbox_min{image.width_ - 1, image.height_ - 1, 0};
  glm::vec3 bbox_max{0, 0, 0};
  for (auto i : points) {
    bbox_max.x = std::max(i.position_.x, bbox_max.x);
    bbox_max.y = std::max(i.position_.y, bbox_max.y);
    bbox_min.x = std::min(i.position_.x, bbox_min.x);
    bbox_min.y = std::min(i.position_.y, bbox_min.y);
  }
  glm::vec3 normal =
      glm::normalize(glm::cross(points[2].position_ - points[0].position_,
                                points[1].position_ - points[0].position_));
  glm::vec3 p;
  //  (P - points[0]) * normal = 0;

  glm::vec3 AB = points[1].position_ - points[0].position_;
  glm::vec3 AC = points[2].position_ - points[0].position_;
  glm::vec3 BC = points[2].position_ - points[1].position_;
  float total_area = glm::length(glm::cross(AB, AC));
  for (p.x = bbox_min.x; p.x < bbox_max.x; p.x++) {
    for (p.y = bbox_min.y; p.y < bbox_max.y; p.y++) {
      // point-normal formular(that's the math way)
      p.z = ((-normal.x * (p.x - points[0].position_.x) -
              normal.y * (p.y - points[0].position_.y)) /
             normal.z) +
            points[0].position_.z;

      if (point_in_triangle(points[0].position_, points[1].position_,
                            points[2].position_, p)) {
        if (zbuffer[int(p.y * image.width_ + p.x)] <= p.z) {
          glm::vec3 color;
          // https://www.zhihu.com/question/38356223/answer/76043922
          // points[0].position_ A colors[0] A.color
          // points[1].position_ B
          // points[2].position_ C
          // total Area = || AB X AC ||
          // p.color = A.color * Area_{PBC} /total_Area + ....
          //        = A.color * || PB * BC|| /total_area + ..

          glm::vec3 BP = p - points[1].position_;
          glm::vec3 CP = p - points[2].position_;
          glm::vec3 AP = p - points[0].position_;
          float PAB_area = glm::length(glm::cross(AP, AB));
          float PBC_area = glm::length(glm::cross(BP, BC));
          float PCA_area = glm::length(glm::cross(CP, -AC));
          color = ((colors[0] * PBC_area) + (colors[1] * PCA_area) +
                   (colors[2] * PAB_area)) /
                  total_area;
          image.set_pixel(int(p.x), int(p.y), color);
          zbuffer[int(p.y * image.width_ + p.x)] = p.z;
        }
      }
    }
  }
}
void triangle(std::array<Vertex, 3> points, float *zbuffer, PPMImage &image,
              const PPMImage &texture) {
  // note: the texture should be flipped vertically
  std::array<glm::vec3, 3> colors;
  colors[0] = texture.get_pixel(int(points[0].texcoords_.y),
                                int(points[0].texcoords_.x));
  colors[1] = texture.get_pixel(int(points[1].texcoords_.y),
                                int(points[1].texcoords_.x));
  colors[2] = texture.get_pixel(int(points[2].texcoords_.y),
                                int(points[2].texcoords_.x));
  triangle(points, zbuffer, image, colors);
}
} // namespace pd

#endif // GEOMETRY_HPP

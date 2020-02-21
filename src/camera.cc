#include "camera.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

pd::Camera::Camera(glm::vec3 origin, glm::vec3 up, glm::vec3 lookat, float vfov,
                   float aspect) {
  origin_ = origin;
  up_ = up;
  lookat_ = lookat;
  vfov_ = vfov;
  aspect_ = aspect;
}

glm::mat4 pd::Camera::look_at() {
  glm::vec3 zaxis = glm::normalize(origin_ - lookat_); // direction vector
  glm::vec3 xaxis = glm::normalize(
      glm::cross(glm::normalize(up_), zaxis)); // camera right vector
  glm::vec3 yaxis = glm::cross(zaxis, xaxis);  // camera up vector

  glm::mat4 trans = glm::mat4(1.0f);
  glm::mat4 rotation = glm::mat4(1.0f);
  trans[3][0] = -origin_.x;
  trans[3][1] = -origin_.y;
  trans[3][2] = -origin_.z;
  rotation[0][0] = xaxis.x;
  rotation[1][0] = xaxis.y;
  rotation[2][0] = xaxis.z;

  rotation[0][1] = yaxis.x;
  rotation[1][1] = yaxis.y;
  rotation[2][1] = yaxis.z;

  rotation[0][2] = zaxis.x;
  rotation[1][2] = zaxis.y;
  rotation[2][2] = zaxis.z;

  //  std::cout << glm::to_string(trans) << '\t' << glm::to_string(rotation)
  //            << std::endl;
  return trans * rotation;
}

glm::mat4 pd::Camera::perspective() {
  glm::mat4 proj{1.0f};
  proj[2][3] = -1.0f / glm::length(lookat_ - origin_);
  return proj;
}
glm::mat4 pd::Camera::ortho() { return glm::mat4{1.0f}; }

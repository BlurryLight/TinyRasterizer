#include "geometry.hpp"
#include "model.h"
#include "ppm.hpp"
#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
using namespace pd;
#include <cstring>
int main(int argc, char *argv[]) {
  std::string f("file.ppm");
  constexpr int width = 800;
  constexpr int height = 800;
  PPMImage image(width, height);
  Pixel tmp{100, 100, 100};
  std::fill(image.image_.begin(), image.image_.end(), tmp);
  std::array<float, width * height> zbuffer;
  zbuffer.fill(std::numeric_limits<float>::lowest());

  Model model("african_head.obj", false);

  glm::vec3 lightdir{0, 0, -1}; // right-hand-coords

  glm::mat4 model_mat(1.0f);
  model_mat = glm::rotate(model_mat, glm::radians(90.0f), glm::vec3(0, 0, 1));
  model_mat = glm::rotate(model_mat, glm::radians(45.0f), glm::vec3(0, 1, 0));

  Camera cam(glm::vec3(10, 0, 100), glm::vec3(0, 1, 0), glm::vec3(0, 0, 0),
             90.0f, float(width) / (float)height);
  glm::mat4 view_mat = cam.look_at();
  for (auto mesh : model.meshes_) {
    std::array<glm::vec3, 3> world_coords;
    for (int i = 0; i < mesh.vertices_.size(); i += 3) {
      auto v0 = mesh.vertices_[mesh.indices_[i]];
      auto v1 = mesh.vertices_[mesh.indices_[i + 1]];
      auto v2 = mesh.vertices_[mesh.indices_[i + 2]];
      v0.position_ = glm::mat3(view_mat) * glm::mat3(model_mat) * v0.position_;
      v1.position_ = glm::mat3(view_mat) * glm::mat3(model_mat) * v1.position_;
      v2.position_ = glm::mat3(view_mat) * glm::mat3(model_mat) * v2.position_;
      glm::vec3 normal = glm::normalize(
          glm::cross(v2.position_ - v0.position_, v1.position_ - v0.position_));
      float intensity = std::abs(glm::dot(normal, lightdir));
      world_coords[0] = {int((v0.position_.x + 1.0) * width / 2.0f),
                         int((v0.position_.y + 1.0) * height / 2.0f),
                         v0.position_.z};
      world_coords[1] = {int((v1.position_.x + 1.0) * width / 2.0f),
                         int((v1.position_.y + 1.0) * height / 2.0f),
                         v1.position_.z};
      world_coords[2] = {int((v2.position_.x + 1.0) * width / 2.0f),
                         int((v2.position_.y + 1.0) * height / 2.0f),
                         v2.position_.z};
      triangle(world_coords, zbuffer.data(), image,
               {intensity * 255.0f, intensity * 255.0f, intensity * 255.0f});
    }
  }
  ppm3_write(f.c_str(), image.width_, image.height_, image.image_);

  return 0;
}

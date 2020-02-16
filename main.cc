#include "geometry.hpp"
#include "model.h"
#include "ppm.hpp"
#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
using namespace pd;
#include <cstring>
int main(int argc, char *argv[]) {
  std::string f("file.ppm");
  constexpr int width = 1600;
  constexpr int height = width;
  PPMImage image(width, height);
  Pixel tmp{100, 100, 100};
  std::fill(image.image_.begin(), image.image_.end(), tmp);
  auto zbuffer = std::unique_ptr<std::array<float, width * height>>(
      new std::array<float, width * height>());
  zbuffer->fill(std::numeric_limits<float>::lowest());

  Model model("african_head.obj", false);

  glm::vec3 lightdir{0, 0, -1}; // right-hand-coords

  glm::mat4 model_mat(1.0f);
  model_mat = glm::translate(model_mat, glm::vec3(0, 0, 0));
  model_mat = glm::rotate(model_mat, glm::radians(90.0f), glm::vec3(0, 0, 1));

  Camera cam(glm::vec3(-0.2, 0, 1), glm::vec3(0, 1, 0), glm::vec3(0, 0, 0),
             90.0f, float(width) / (float)height);
  glm::mat4 view_mat = cam.look_at();
  //  glm::mat4 proj_mat = cam.ortho(); // ortho projection
  glm::mat4 proj_mat = cam.perspective(); // perspective projection
  auto render_man = [&]() -> void {
    for (auto mesh : model.meshes_) {
      std::array<glm::vec3, 3> world_coords;
      glm::vec3 diffuse_color = 0.3f * glm::vec3(255.0f, 255.0f, 0.0f);
      for (int i = 0; i < mesh.vertices_.size(); i += 3) {
        auto v0 = mesh.vertices_[mesh.indices_[i]];
        auto v1 = mesh.vertices_[mesh.indices_[i + 1]];
        auto v2 = mesh.vertices_[mesh.indices_[i + 2]];
        // clang-format off
      glm::vec4 tmp;
      tmp = proj_mat*view_mat * model_mat * glm::vec4(v0.position_, 1.0f);
      v0.position_ = glm::vec3(tmp.x/tmp.w,tmp.y/tmp.w,tmp.z/tmp.w);
      tmp = proj_mat*view_mat * model_mat * glm::vec4(v1.position_, 1.0f);
      v1.position_ = glm::vec3(tmp.x/tmp.w,tmp.y/tmp.w,tmp.z/tmp.w);
      tmp = proj_mat*view_mat * model_mat * glm::vec4(v2.position_, 1.0f);
      v2.position_ = glm::vec3(tmp.x/tmp.w,tmp.y/tmp.w,tmp.z/tmp.w);
        // clang-format on
        glm::vec3 normal = glm::normalize(glm::cross(
            v2.position_ - v0.position_, v1.position_ - v0.position_));
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

        glm::vec3 lightColor = glm::vec3{255.0f} * intensity;
        glm::vec3 color = diffuse_color + lightColor;
        // HDR mapping
        color = 1.5f * color / (color + 255.0f) * glm::vec3(255.0f);

        triangle(world_coords, zbuffer->data(), image, color);
      }
    }
  };
  render_man();
  model_mat = glm::mat4(1.0f);
  model_mat = glm::rotate(model_mat, glm::radians(90.0f), glm::vec3(0, 0, 1));
  model_mat = glm::translate(model_mat, glm::vec3(-10, 1.0, -10));
  render_man();
  model_mat = glm::mat4(1.0f);
  model_mat = glm::rotate(model_mat, glm::radians(90.0f), glm::vec3(0, 0, 1));
  model_mat = glm::rotate(model_mat, glm::radians(45.0f), glm::vec3(1, 0, 0));
  model_mat = glm::translate(model_mat, glm::vec3(0.7, 0.5, -2));
  render_man();
  ppm3_write(f.c_str(), image.width_, image.height_, image.image_);

  return 0;
}

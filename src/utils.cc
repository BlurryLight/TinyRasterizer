#include "utils.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
using namespace pd;
static std::random_device rd;
static std::default_random_engine generator(rd());
static std::uniform_int_distribution<> int_dis(0, 255);
static std::uniform_real_distribution<> real_dis(0.1, 0.95);

void pd::render_random_triangle(int width, int height, float *zbuffer,
                                PPMImage &image) {
  std::array<Vertex, 3> vertices;
  int z = 100;
  vertices[0].position_ =
      glm::vec3(width * real_dis(generator), height * real_dis(generator),
                float(z * real_dis(generator)));
  vertices[1].position_ =
      glm::vec3(width * real_dis(generator), height * real_dis(generator),
                float(z * real_dis(generator)));
  vertices[2].position_ =
      glm::vec3(width * real_dis(generator), height * real_dis(generator),
                float(z * real_dis(generator)));
  std::array<glm::vec3, 3> colors;
  colors[0] = {int_dis(generator), int_dis(generator), int_dis(generator)};
  colors[1] = {int_dis(generator), int_dis(generator), int_dis(generator)};
  colors[2] = {int_dis(generator), int_dis(generator), int_dis(generator)};
  triangle(vertices, zbuffer, image, colors);
}

void pd::render_quad(float *zbuffer, PPMImage &image, const PPMImage *texture) {
  // OpenGL NDC coords here
  // OpenGL NDC coords is flipped vertically in my soft render
  // because the ppm write outputs from top to down, so the (0,0) is the
  // left-top
  std::array<Vertex, 3> triangle1;
  std::array<Vertex, 3> triangle2;
  // clang-format off
  triangle1[0].position_ = {-1.0f,1.0f,0.0f}; triangle1[0].texcoords_ = {0.0f,1.0f};//left-top
  triangle1[1].position_ = {-1.0f,-1.0f,0.0f}; triangle1[1].texcoords_ = {0.0f,0.0f};//left-bottom
  triangle1[2].position_ = {1.0f,-1.0f,0.0f}; triangle1[2].texcoords_ = {1.0f,0.0f};//right-bottom
  //second triangle
  triangle2[0].position_ = triangle1[2].position_;triangle2[0].texcoords_ = triangle1[2].texcoords_;
  triangle2[1].position_ = {1.0f,1.0f,0.0f};triangle2[1].texcoords_ = {1.0f,1.0f}; //right-top
  triangle2[2].position_ = triangle1[0].position_;triangle2[2].texcoords_ = triangle1[0].texcoords_;
  // clang-format on
  // now turn them into screen coords
  // swap x,y, scale as 0.5 then translate (0.5,0.5)
  glm::mat4 model(1.0f);
  model[0][0] = 0.0;
  model[0][1] = 0.5;
  model[1][1] = 0.0;
  model[1][0] = -0.5;
  model[3][0] = 0.5;
  model[3][1] = 0.5;
  glm::mat4 trans = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 1.0f));
  trans = glm::rotate(trans, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  trans = glm::translate(trans, glm::vec3(0.5, 0.5, 0.0));
  auto coords_transform = [&](std::array<Vertex, 3> &tri) {
    for (auto &i : tri) {
      auto tmp = trans * model * glm::vec4(i.position_, 1.0f);
      i.position_ = tmp;
      i.position_.x = i.position_.x * image.width_;
      i.position_.y = i.position_.y * image.height_;
    }
  };
  coords_transform(triangle1);
  coords_transform(triangle2);
  if (!texture) {
    std::array<glm::vec3, 3> colors;
    colors[0] = YELLOW;
    colors[1] = BLUE;
    colors[2] = PURPLE;
    triangle(triangle1, zbuffer, image, colors);
    triangle(triangle2, zbuffer, image, colors);
  } else {
    triangle(triangle1, zbuffer, image, *texture);
    triangle(triangle2, zbuffer, image, *texture);
  }
}

void pd::render_triangle(int width, int height, float *zbuffer,
                         PPMImage &image) {
  std::array<Vertex, 3> vertices;
  int z = 100;
  vertices[0].position_ = {0, 0, 0};
  vertices[1].position_ = {0, 1, 0};
  vertices[2].position_ = {1, 0, 0};
  std::array<glm::vec3, 3> colors;
  colors[0] = BLUE;
  colors[1] = YELLOW;
  colors[2] = PURPLE;

  for (auto &i : vertices) {

    i.position_.x = i.position_.x * image.width_;
    i.position_.y = i.position_.y * image.height_;
  }
  triangle(vertices, zbuffer, image, colors);
}

void pd::render_man(int width, int height, float *zbuffer, PPMImage &image,
                    const PPMImage *texture) {
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

      glm::vec3 lightColor = glm::vec3{255.0f} * intensity;
      // FIXME:
      if (texture) {

        //        std::array<glm::vec3, 3> colors;
        //        //        colors.fill(lightColor);
        //        colors[0] += static_cast<glm::vec3>(texture->get_pixel(
        //            int(v0.texcoords_.x * width), int(v0.texcoords_.y *
        //            height)));
        //        colors[1] += static_cast<glm::vec3>(texture->get_pixel(
        //            int(v1.texcoords_.x * width), int(v1.texcoords_.y *
        //            height)));
        //        colors[2] += static_cast<glm::vec3>(texture->get_pixel(
        //            int(v2.texcoords_.x * width), int(v2.texcoords_.y *
        //            height)));
        //        //         tone mapping
        //        for (auto color : colors) {
        //          color = 1.5f * color / (color + 255.0f) * glm::vec3(255.0f);
        //        }
        //        triangle(world_coords, zbuffer, image, texture);
      } else {
        glm::vec3 color = lightColor;
        color += diffuse_color;

        // HDR mapping
        color = 1.5f * color / (color + 255.0f) * glm::vec3(255.0f);
        triangle(world_coords, zbuffer, image, color);
      }
    }
  }
}

void pd::render_cube(float *zbuffer, PPMImage &image, const PPMImage *texture) {
  // clang-format off
  float vertices[] = {
      //vertex  -  normal -  texture
      // back face
      -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
      1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
      1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
      1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
      -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
      -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
      // front face
      -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
      1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
      1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
      1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
      -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
      -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
      // left face
      -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
      -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
      -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
      -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
      -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
      -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
      // right face
      1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
      1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
      1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
      1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
      1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
      1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
      // bottom face
      -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
      1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
      1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
      1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
      -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
      -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
      // top face
      -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
      1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
      1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
      1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
      -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
      -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
  };
  // clang-format on
  using tri_verts = std::array<Vertex, 3>;
  // triangles
  std::vector<tri_verts> tris; // should have 12 elemets( 6 faces * 2 triangles)

  int stride = 8;
  // like glBufferData
  auto setup_tris = [&](std::array<Vertex, 3> &verts, int i, int stride) {
    for (int j = 0; j < 3; j++) {
      verts[j].position_ = {vertices[i], vertices[i + 1], vertices[i + 2]};
      verts[j].normal_ = {vertices[i + 3], vertices[i + 4], vertices[i + 5]};
      verts[j].texcoords_ = {vertices[i + 6], vertices[i + 7]};
      i += stride;
    }
  };
  for (int i = 0; i < sizeof(vertices) / sizeof(float); i += 3 * stride) {
    tri_verts tmp;
    setup_tris(tmp, i, stride);
    tris.push_back(tmp);
  }
  // transform
  // NDC to my own coords
  glm::mat4 NDCtrans(1.0f);
  NDCtrans[0][0] = 0.0;
  NDCtrans[0][1] = 0.5;
  NDCtrans[1][1] = 0.0;
  NDCtrans[1][0] = -0.5;
  NDCtrans[3][0] = 0.5;
  NDCtrans[3][1] = 0.5;

  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(1.1, 1.0, 0.0));
  model = glm::scale(model, glm::vec3(0.4));
  Camera cam({0.7, 0.0, 2}, {0, 1, 0}, {0.0, 0.0, 0}, 90.0f, 1.0f);
  //  glm::mat4 proj = glm::mat4(1.0f); // ortho
  glm::mat4 proj = cam.perspective();
  glm::mat4 view = cam.look_at();
  auto transform = [&](tri_verts &verts) {
    for (auto &i : verts) {
      auto tmp =
          proj * view * model * (NDCtrans * glm::vec4(i.position_, 1.0f));
      i.position_ = {tmp.x / tmp.w, tmp.y / tmp.w, tmp.z / tmp.w};
      i.position_.x = i.position_.x * image.width_;
      i.position_.y = i.position_.y * image.height_;
    }
  };

  for (auto &i : tris) {
    transform(i);
  }
  if (!texture) {
    std::array<glm::vec3, 3> colors;
    colors[0] = PURPLE;
    colors[1] = YELLOW;
    colors[2] = BLUE;
    for (const auto &i : tris) {
      triangle(i, zbuffer, image, colors);
    }
  } else {
    for (const auto &i : tris) {
      triangle(i, zbuffer, image, *texture);
    }
  }
}

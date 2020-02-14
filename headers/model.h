#ifndef MODEL_H
#define MODEL_H
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>
namespace pd {

struct Vertex {
  glm::vec3 position_;
  glm::vec3 normal_;
  glm::vec2 texcoords_;
};
struct Texture {
  std::string type;
  std::string path;
};

struct Mesh {
  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  std::vector<Texture> textures_;
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures) {
    vertices_ = vertices;
    indices_ = indices;
    textures_ = textures;
  }
};

class Model {
public:
  //  std::vector<Texture> textures_loaded_;
  std::vector<Mesh> meshes_;
  bool gamma_correction = false;

  Model(const std::string &path, bool gamma) {
    gamma_correction = gamma;
    load_model(path);
  }

private:
  void load_model(const std::string &path);
  void load_node(aiNode *node, const aiScene *scene);
  Mesh load_mesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> load_material_textures(aiMaterial *mat,
                                              aiTextureType type,
                                              std::string type_name);
};

// https://github.com/ssloy/tinyrenderer/blob/68a5ae382135d679891423fb5285fdd582ca389d/model.h
// I copy it here to varify whether my code is working correctly
class Model2 {
private:
  std::vector<glm::vec3> verts_;
  std::vector<std::vector<int>> faces_;

public:
  Model2(const char *filename);
  ~Model2();
  int nverts();
  int nfaces();
  glm::vec3 vert(int i);
  std::vector<int> face(int idx);
};

};     // namespace pd
#endif // MODEL_H

#include "model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace pd;
void pd::Model::load_model(const std::string &path) {
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs |
                                  aiProcess_CalcTangentSpace);
  //      importer.ReadFile(path, aiProcess_FlipUVs);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cerr << "load model failed" << importer.GetErrorString() << std::endl;
    return;
  }
  load_node(scene->mRootNode, scene);
}

void pd::Model::load_node(aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes_.push_back(load_mesh(mesh, scene));
  }
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    load_node(node->mChildren[i], scene);
  }
}

pd::Mesh pd::Model::load_mesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vec; // tmp vector to store data
    // position
    vec.x = mesh->mVertices[i].x;
    vec.y = mesh->mVertices[i].y;
    vec.z = mesh->mVertices[i].z;
    vertex.position_ = vec;
    // normal
    vec.x = mesh->mNormals[i].x;
    vec.y = mesh->mNormals[i].y;
    vec.z = mesh->mNormals[i].z;
    vertex.normal_ = vec;
    // texture coords
    if (mesh->mTextureCoords[0]) // if has texture[0]
    // note we just take texture[0]
    // maximum is 8
    {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.texcoords_ = vec;
    } else {
      vertex.texcoords_ = glm::vec2{0, 0};
    }
    // TODO: tangent
    // TODO: bitangent
    vertices.push_back(vertex);
  }
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }
  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
  std::vector<Texture> diffuse_maps =
      load_material_textures(material, aiTextureType_DIFFUSE, "diffuse");
  std::vector<Texture> specular_maps =
      load_material_textures(material, aiTextureType_SPECULAR, "specular");
  // should use height or normal?
  std::vector<Texture> normal_maps =
      load_material_textures(material, aiTextureType_NORMALS, "normal");
  textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
  textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
  textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
  return Mesh(vertices, indices, textures);
}

std::vector<Texture> pd::Model::load_material_textures(aiMaterial *mat,
                                                       aiTextureType type,
                                                       std::string type_name) {
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    Texture texture;
    texture.type = type_name;
    texture.path = str.C_Str();
    textures.push_back(texture);
  }
  return textures;
}

pd::Model2::Model2(const char *filename) : verts_(), faces_() {
  std::ifstream in;
  in.open(filename, std::ifstream::in);
  if (in.fail())
    return;
  std::string line;
  while (!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());
    char trash;
    if (!line.compare(0, 2, "v ")) {
      iss >> trash;
      glm::vec3 v;
      iss >> v.x >> v.y >> v.z;
      verts_.push_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      std::vector<int> f;
      int itrash, idx;
      iss >> trash;
      while (iss >> idx >> trash >> itrash >> trash >> itrash) {
        idx--; // in wavefront obj all indices start at 1, not zero
        f.push_back(idx);
      }
      faces_.push_back(f);
    }
  }
  std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

Model2::~Model2() {}

int Model2::nverts() { return (int)verts_.size(); }

int Model2::nfaces() { return (int)faces_.size(); }

std::vector<int> Model2::face(int idx) { return faces_[idx]; }

glm::vec3 Model2::vert(int i) { return verts_[i]; }

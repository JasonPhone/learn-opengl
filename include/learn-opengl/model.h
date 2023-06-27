/**
 * @file model.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Model and Mesh classes to receive (assimp or other library) imported
 * models.
 * @version 0.1
 * @date 2022-12-05
 *
 */
#pragma once
#include <cassert>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shader;

/// @brief Vertex pos, normal and texture coord.
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;
};
/// @brief Texture ID, type and path to file.
struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

class Mesh {
 public:
  // TODO move?
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures)
      : mVertices{vertices}, mIndices{indices}, mTextures{textures} {
    setup_mesh();
  }
  /// @brief Draw the mesh using provided shader.
  void draw(const Shader& shader);
  void drawInstanced(const Shader& shader, const int instancedNum);

  std::vector<Vertex> mVertices;
  std::vector<unsigned int> mIndices;
  std::vector<Texture> mTextures;
  /// @brief Obj and buffers to draw this mesh.
  unsigned int mVAO, mVBO, mEBO;

 private:
  /// @brief Setup mesh object and fill the buffer obj and array obj.
  void setup_mesh();
};

/// @brief Store a model.
class Model {
 public:
  Model(const char* path) { load_model(path); }
  void draw(const Shader& shader) {
    for (auto&& mesh : mMeshes) mesh.draw(shader);
  }
  void drawInstanced(const Shader& shader, const int instanceNum) {
    for (auto&& mesh : mMeshes) mesh.drawInstanced(shader, instanceNum);
  }

 private:
  void load_model(const std::string& path, bool triangulate = true);
  // void process_node(aiNode *node, const aiScene *scene);
  // Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
  // std::vector<Texture> load_material_textures(aiMaterial *mat,
  //                                             aiTextureType tex_type,
  //                                             const std::string &type_name);

 public:
  std::vector<Mesh> mMeshes;
  std::vector<Texture> mLoadedTexture;
  /// @brief Path without filename.
  std::string mDirectory;
};

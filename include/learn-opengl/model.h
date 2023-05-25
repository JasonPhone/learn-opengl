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

  std::vector<Vertex> mVertices;
  std::vector<unsigned int> mIndices;
  std::vector<Texture> mTextures;

 private:
  /// @brief Obj and buffers to draw this mesh.
  unsigned int mVAO, mVBO, mEBO;
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

 private:
  void load_model(const std::string& path, bool triangulate = true);
  // void process_node(aiNode *node, const aiScene *scene);
  // Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
  // std::vector<Texture> load_material_textures(aiMaterial *mat,
  //                                             aiTextureType tex_type,
  //                                             const std::string &type_name);

  std::vector<Mesh> mMeshes;
  std::vector<Texture> mLoadedTexture;
  /// @brief Path without filename.
  std::string mDirectory;
};

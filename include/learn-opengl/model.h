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
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// #include "learn-opengl/shader.h"
class Shader;

// To receive vertices
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coord;
};
// To receive textures
struct Texture {
  unsigned int id;
  std::string type;
  aiString path;
};

class Mesh {
 public:
  // TODO move?
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures)
      : m_vertices{vertices}, m_indices{indices}, m_textures{textures} {
    setup_mesh();
  }
  /**
   * @brief Draw the mesh using provided shader.
   */
  void draw(const Shader &shader);

  std::vector<Vertex> m_vertices;
  std::vector<unsigned int> m_indices;
  std::vector<Texture> m_textures;

 private:
  unsigned int m_VAO, m_VBO, m_EBO;
  /**
   * @brief Setup mesh object and fill the buffer obj and array obj.
   */
  void setup_mesh();
};

/**
 * @brief Store a model inported from assimp
 *
 */
class Model {
 public:
  Model(const char *path) { load_model(path); }
  void draw(const Shader &shader) {
    for (auto &&mesh : m_meshes) mesh.draw(shader);
  }

 private:
  void load_model(const std::string &path);
  void process_node(aiNode *node, const aiScene *scene);
  Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> load_material_textures(aiMaterial *mat,
                                              aiTextureType tex_type,
                                              const std::string &type_name);

  std::vector<Mesh> m_meshes;
  std::vector<Texture> m_loaded_texture;
  std::string m_directory;
};
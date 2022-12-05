/**
 * @file mesh.h
 * @author ja50n (zs_feng@qq.com)
 * @brief Mesh class to receive (assimp or other library) imported models.
 * @version 0.1
 * @date 2022-12-05
 *
 */
#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

// To receive vertices
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tex_coord;
};
// To receive textures
struct Texture {
  unsigned int id;
  std::string type;
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
   * @brief Draw the mesh.
   */
  void draw();

  std::vector<Vertex> m_vertices;
  std::vector<unsigned int> m_indices;
  std::vector<Texture> m_textures;

 private:
  unsigned int VAO, VBO, EBO;
  /**
   * @brief Setup mesh object and fill the buffer obj and array obj.
   */
  void setup_mesh();
};
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "learn-opengl/mesh.h"
#include "learn-opengl/shader.h"

/**
 * @brief Bind OpenGL object buffers and array buffers
 *
 */
void Mesh::setup_mesh() {
  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);
  glGenBuffers(1, &m_EBO);

  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
               &m_vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int),
               &m_indices[0], GL_STATIC_DRAW);

  // Vertex attrib position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  // Vertex attrib normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, normal));
  // Vertex attrib texture coordinates
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, tex_coord));
}

void Mesh::draw(Shader const& shader) {
  unsigned int num_tex_diffuse = 1;
  unsigned int num_tex_specular = 1;
  for (unsigned int i = 0; i < m_textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    // Generate the texture name
    std::string number;
    std::string name = m_textures[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(num_tex_diffuse++);
    else if (name == "texture_specular")
      number = std::to_string(num_tex_specular++);

    shader.set_int(("material." + name + number).c_str(), i);
    glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  // Draw the mesh
  glBindVertexArray(m_VAO);
  glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
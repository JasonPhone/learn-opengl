#pragma once
#include <glm/glm.hpp>

// To receive vertices
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tex_coord;
};
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include "glm/fwd.hpp"
#include "learn-opengl/stringformat.h"
#include "learn-opengl/gl_utility.h"
#include "learn-opengl/image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "learn-opengl/Model.h"
#include "learn-opengl/Shader.h"

static void processModel(const tinyobj::attrib_t &attrib,
                         const std::vector<tinyobj::shape_t> &shapes,
                         const std::vector<tinyobj::material_t> &materials,
                         std::vector<Mesh> &meshes, const std::string &dir);
void Mesh::setupMesh() {
  glGenVertexArrays(1, &mVAO);
  glGenBuffers(1, &mVBO);
  glGenBuffers(1, &mEBO);

  glBindVertexArray(mVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);

  glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex),
               &mVertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int),
               &mIndices[0], GL_STATIC_DRAW);

  // Vertex attrib position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // Vertex attrib normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  // Vertex attrib texture coordinates
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texCoord));
  glBindVertexArray(0);
}
void Mesh::draw(Shader const &shader) {
  unsigned int num_tex_diffuse = 1;
  unsigned int num_tex_specular = 1;
  unsigned int num_tex_normal = 1;
  unsigned int num_tex_height = 1;
  for (unsigned int i = 0; i < mTextures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    // Generate the texture name
    std::string number;
    std::string name = mTextures[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(num_tex_diffuse++);
    else if (name == "texture_specular")
      number = std::to_string(num_tex_specular++);
    else if (name == "texture_normal")
      number = std::to_string(num_tex_normal++);
    else if (name == "texture_height")
      number = std::to_string(num_tex_height++);

    shader.setInt(("material." + name + number).c_str(), i);
    // LOG("Set texture", ("material." + name + number).c_str());
    glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  // Draw the mesh
  glBindVertexArray(mVAO);
  glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
void Mesh::drawInstanced(Shader const &shader, const int instanceNum) {
  unsigned int num_tex_diffuse = 1;
  unsigned int num_tex_specular = 1;
  unsigned int num_tex_normal = 1;
  unsigned int num_tex_height = 1;
  for (unsigned int i = 0; i < mTextures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    // Generate the texture name
    std::string number;
    std::string name = mTextures[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(num_tex_diffuse++);
    else if (name == "texture_specular")
      number = std::to_string(num_tex_specular++);
    else if (name == "texture_normal")
      number = std::to_string(num_tex_normal++);
    else if (name == "texture_height")
      number = std::to_string(num_tex_height++);

    shader.setInt(("material." + name + number).c_str(), i);
    // LOG("Set texture", ("material." + name + number).c_str());
    glBindTexture(GL_TEXTURE_2D, mTextures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  // Draw the mesh
  glBindVertexArray(mVAO);
  glDrawElementsInstanced(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0,
                          instanceNum);
  glBindVertexArray(0);
}

void Model::loadModel(const std::string &path, bool triangulate) {
  LOG << "Loading " << path << std::endl;
  mDirectory = path.substr(0, path.find_last_of('/'));
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warn;
  std::string err;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                              path.c_str(), mDirectory.c_str(), triangulate);

  if (!warn.empty()) WARN << warn << std::endl;
  if (!err.empty()) ERR << err << std::endl;
  if (!ret) {
    ERR << "Failed to load/parse .obj.\n";
  } else {
    processModel(attrib, shapes, materials, mMeshes, mDirectory);
  }
}

static void processModel(const tinyobj::attrib_t &attrib,
                         const std::vector<tinyobj::shape_t> &shapes,
                         const std::vector<tinyobj::material_t> &materials,
                         std::vector<Mesh> &meshes, const std::string &dir) {
  LOG << "# of vertices  : " << (attrib.vertices.size() / 3) << std::endl;
  LOG << "# of normals   : " << (attrib.normals.size() / 3) << std::endl;
  LOG << "# of texcoords : " << (attrib.texcoords.size() / 2) << std::endl;
  LOG << "# of shapes    : " << shapes.size() << std::endl;
  LOG << "# of materials : " << materials.size() << std::endl;

  std::vector<std::vector<Texture>> globalTextures;
  // Materials.
  LOG << "Loading materials.\n";
  for (size_t idxMat = 0; idxMat < materials.size(); idxMat++) {
    std::vector<Texture> textures;
    LOG << "Material name " << materials[idxMat].name << ": ";
    Texture tex;
    // Diffuse.
    if (!materials[idxMat].diffuse_texname.empty()) {
      LOG << "diffuse ";
      tex.id =
          loadTexture((dir + "/" + materials[idxMat].diffuse_texname).c_str());
      tex.type = "texture_diffuse";
      tex.path = materials[idxMat].diffuse_texname;
      textures.push_back(tex);
    }
    // Specular.
    if (!materials[idxMat].specular_texname.empty()) {
      LOG << "specular ";
      tex.id =
          loadTexture((dir + "/" + materials[idxMat].specular_texname).c_str());
      tex.type = "texture_specular";
      tex.path = materials[idxMat].specular_texname;
      textures.push_back(tex);
    }
    // Normal.
    if (!materials[idxMat].bump_texname.empty()) {
      LOG << "normal ";
      tex.id =
          loadTexture((dir + "/" + materials[idxMat].bump_texname).c_str());
      tex.type = "texture_normal";
      tex.path = materials[idxMat].bump_texname;
      textures.push_back(tex);
    }
    globalTextures.push_back(textures);
    LOG << "\n";
  }

  for (size_t i = 0; i < shapes.size(); i++) {
    auto curShape = shapes[i];
    LOG << stringFormat("Loading shape %s(%d)\n", curShape.name.c_str(), i);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> localTextures;
    std::map<unsigned int, unsigned int> idxGlobalToLocal;

    for (size_t idx = 0; idx < curShape.mesh.indices.size(); idx++) {
      tinyobj::index_t index = curShape.mesh.indices[idx];
      Vertex vert;
      glm::vec3 v3;
      // Position.
      v3.x = attrib.vertices[index.vertex_index * 3 + 0];
      v3.y = attrib.vertices[index.vertex_index * 3 + 1];
      v3.z = attrib.vertices[index.vertex_index * 3 + 2];
      vert.position = v3;
      // Normal.
      v3.x = attrib.normals[index.normal_index * 3 + 0];
      v3.y = attrib.normals[index.normal_index * 3 + 1];
      v3.z = attrib.normals[index.normal_index * 3 + 2];
      vert.normal = v3;
      // Tex coord.
      glm::vec2 v2;
      v2.x = attrib.texcoords[index.texcoord_index * 2 + 0];
      v2.y = attrib.texcoords[index.texcoord_index * 2 + 1];
      vert.texCoord = v2;

      if (idxGlobalToLocal.find(index.vertex_index) == idxGlobalToLocal.end()) {
        // A new index, a new vertex.
        idxGlobalToLocal[index.vertex_index] = vertices.size();
        vertices.push_back(vert);
      }
      indices.push_back(idxGlobalToLocal[index.vertex_index]);
    }
    std::set<int> matIds;
    for (size_t idxMat = 0; idxMat < curShape.mesh.material_ids.size();
         idxMat++) {
      auto id = curShape.mesh.material_ids[idxMat];
      if (matIds.find(id) == matIds.end()) {
        matIds.insert(id);
        localTextures.insert(localTextures.end(), globalTextures[id].begin(),
                             globalTextures[id].end());
      }
    }
    meshes.push_back(Mesh{vertices, indices, localTextures});
  }
}

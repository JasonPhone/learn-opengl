#include "learn-opengl/model.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

#include "learn-opengl/shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static unsigned int texture_from_file(const char *path,
                                      const std::string &directory,
                                      bool gamma = false);

// Functions for Mesh
// ------------------
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // Vertex attrib normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  // Vertex attrib texture coordinates
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, tex_coord));
}

void Mesh::draw(Shader const &shader) {
  unsigned int num_tex_diffuse = 1;
  unsigned int num_tex_specular = 1;
  unsigned int num_tex_normal = 1;
  unsigned int num_tex_height = 1;
  for (unsigned int i = 0; i < m_textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    // Generate the texture name
    std::string number;
    std::string name = m_textures[i].type;
    if (name == "texture_diffuse")
      number = std::to_string(num_tex_diffuse++);
    else if (name == "texture_specular")
      number = std::to_string(num_tex_specular++);
    else if (name == "texture_normal")
      number = std::to_string(num_tex_normal++);
    else if (name == "texture_height")
      number = std::to_string(num_tex_height++);

    shader.set_int(("material." + name + number).c_str(), i);
    // LOG("Set texture", ("material." + name + number).c_str());
    glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  // Draw the mesh
  glBindVertexArray(m_VAO);
  glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

// Functions for Model
// -------------------

void Model::load_model(const std::string &path) {
  // Assimp::Importer import;
  // const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate);
  // // import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  // if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
  //     !scene->mRootNode) {
  //   std::cerr << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
  //   m_directory.clear();
  //   return;
  // }
  // m_directory = path.substr(0, path.find_last_of('/'));

  // process_node(scene->mRootNode, scene);
}

/**
 * @brief
 *
 * @param node
 * @param scene Needed to find the real meshes through indices.
 */
// void Model::process_node(aiNode *node, const aiScene *scene) {
// for (int i = 0; i < node->mNumMeshes; i++) {
//   // There is a index table in each node
//   aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
//   m_meshes.push_back(process_mesh(mesh, scene));
// }
// for (int i = 0; i < node->mNumChildren; i++) {
//   process_node(node->mChildren[i], scene);
// }
// }

// Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene) {
//   std::vector<Vertex> vertices;
//   std::vector<unsigned int> indices;
//   std::vector<Texture> textures;
//   for (int i = 0; i < mesh->mNumVertices; i++) {
//     Vertex vert;
//     // 1 Process vertex position, normal and tex coord
//     //   Position
//     glm::vec3 vec;  // Make compatible with data from assimp
//     vec.x = mesh->mVertices[i].x;
//     vec.y = mesh->mVertices[i].y;
//     vec.z = mesh->mVertices[i].z;
//     vert.position = vec;
//     //   Normal
//     vec.x = mesh->mNormals[i].x;
//     vec.y = mesh->mNormals[i].y;
//     vec.z = mesh->mNormals[i].z;
//     vert.normal = vec;
//     //   Tex coord
//     glm::vec2 tec{0};
//     // We only need the first texture coord, although assimp supports up to 8
//     if (mesh->mTextureCoords[0]) {
//       // Tex coord exists
//       tec.x = mesh->mTextureCoords[0][i].x;
//       tec.y = mesh->mTextureCoords[0][i].y;
//     }
//     vert.tex_coord = tec;
//     vertices.push_back(vert);
//   }
//   // 2 Process indices
//   for (int i = 0; i < mesh->mNumFaces; i++) {
//     aiFace face = mesh->mFaces[i];
//     for (int j = 0; j < face.mNumIndices; j++) {
//       indices.push_back(face.mIndices[j]);
//     }
//   }
//   // 3 Material and texture
//   if (mesh->mMaterialIndex >= 0) {
//     // Material exists
//     aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
//     // 3.1 Diffuse
//     std::vector<Texture> diffuse_maps =
//         load_material_textures(mat, aiTextureType_DIFFUSE,
//         "texture_diffuse");
//     textures.insert(textures.end(), diffuse_maps.begin(),
//     diffuse_maps.end());
//     // 3.2 Specular
//     std::vector<Texture> specular_maps =
//         load_material_textures(mat, aiTextureType_SPECULAR,
//         "texture_specular");
//     textures.insert(textures.end(), specular_maps.begin(),
//     specular_maps.end());
//     // 3.3 Normal
//     std::vector<Texture> normal_maps =
//         load_material_textures(mat, aiTextureType_HEIGHT, "texture_normal");
//     textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
//     // 3.4 Height
//     std::vector<Texture> height_maps =
//         load_material_textures(mat, aiTextureType_AMBIENT, "texture_height");
//     textures.insert(textures.end(), height_maps.begin(), height_maps.end());
//   }
//   return Mesh{vertices, indices, textures};
// }

// std::vector<Texture> Model::load_material_textures(
//     aiMaterial *mat, aiTextureType tex_type, const std::string &type_name) {
//   LOG << "Load textures begin " << type_name.c_str();
//   std::vector<Texture> textures;
//   for (int i = 0; i < mat->GetTextureCount(tex_type); i++) {
//     aiString str;
//     mat->GetTexture(tex_type, i, &str);
//     bool b_tex_loaded = false;
//     for (int j = 0; j < m_loaded_texture.size(); j++) {
//       if (std::strcmp(m_loaded_texture[j].path.C_Str(), str.C_Str()) == 0) {
//         // This texture is already loaded.
//         textures.push_back(m_loaded_texture[j]);
//         b_tex_loaded = true;
//         break;
//       }
//     }
//     if (b_tex_loaded == false) {
//       Texture tex;
//       // NOTE We suppose texture paths are all relative to model file.
//       LOG << "Load texture " << str.C_Str();
//       tex.id = texture_from_file(str.C_Str(), m_directory);
//       tex.type = type_name;
//       tex.path = str.C_Str();
//       textures.push_back(tex);
//       m_loaded_texture.push_back(tex);
//     }
//   }
//   return textures;
// }
static unsigned int texture_from_file(const char *path,
                                      const std::string &directory,
                                      bool gamma) {
  std::string filename(path);
  filename = directory + '/' + filename;

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cerr << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }
  return textureID;
}
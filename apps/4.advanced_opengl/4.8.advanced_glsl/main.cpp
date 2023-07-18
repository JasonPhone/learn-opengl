#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learn-opengl/Shader.h"
#include "learn-opengl/Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "learn-opengl/gl_utility.h"

#include <iostream>
#include <vector>
#include <map>

// Settings
constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;
// Timing
float deltaTime = 0.0f;
float last_frame = 0.0f;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;
bool first_mouse = true;

// Vertices
// clang-format off
float cube_vert_pos[] = {
  // Back face
  -0.5f, -0.5f, -0.5f, // Bottom-left
   0.5f,  0.5f, -0.5f, // top-right
   0.5f, -0.5f, -0.5f, // bottom-right
   0.5f,  0.5f, -0.5f, // top-right
  -0.5f, -0.5f, -0.5f, // bottom-left
  -0.5f,  0.5f, -0.5f, // top-left
  // Front face
  -0.5f, -0.5f,  0.5f, // bottom-left
   0.5f, -0.5f,  0.5f, // bottom-right
   0.5f,  0.5f,  0.5f, // top-right
   0.5f,  0.5f,  0.5f, // top-right
  -0.5f,  0.5f,  0.5f, // top-left
  -0.5f, -0.5f,  0.5f, // bottom-left
  // Left face
  -0.5f,  0.5f,  0.5f, // top-right
  -0.5f,  0.5f, -0.5f, // top-left
  -0.5f, -0.5f, -0.5f, // bottom-left
  -0.5f, -0.5f, -0.5f, // bottom-left
  -0.5f, -0.5f,  0.5f, // bottom-right
  -0.5f,  0.5f,  0.5f, // top-right
  // Right face
   0.5f,  0.5f,  0.5f, // top-left
   0.5f, -0.5f, -0.5f, // bottom-right
   0.5f,  0.5f, -0.5f, // top-right
   0.5f, -0.5f, -0.5f, // bottom-right
   0.5f,  0.5f,  0.5f, // top-left
   0.5f, -0.5f,  0.5f, // bottom-left
  // Bottom face
  -0.5f, -0.5f, -0.5f, // top-right
   0.5f, -0.5f, -0.5f, // top-left
   0.5f, -0.5f,  0.5f, // bottom-left
   0.5f, -0.5f,  0.5f, // bottom-left
  -0.5f, -0.5f,  0.5f, // bottom-right
  -0.5f, -0.5f, -0.5f, // top-right
  // Top face
  -0.5f,  0.5f, -0.5f, // top-left
   0.5f,  0.5f,  0.5f, // bottom-right
   0.5f,  0.5f, -0.5f, // top-right
   0.5f,  0.5f,  0.5f, // bottom-right
  -0.5f,  0.5f, -0.5f, // top-left
  -0.5f,  0.5f,  0.5f  // bottom-left
};
// clang-format on
void framebuffer_size_callback(GLFWwindow *window, int w, int h) {
  glViewport(0, 0, w, h);
}
void process_input(GLFWwindow *window) {
  // Close window on Esc pressed
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  // Camera move
  float cam_speed = camera.moveSpeed() * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::DOWN, deltaTime);
}
void mouse_move_callback(GLFWwindow *window, double xpos, double ypos) {
  camera.turn(xpos, ypos);
}
void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.zoom(yoffset);
}

int main() {
  // Initialize and configure glfw
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  camera.setMoveSpeed(3);

  // glfw window creation
  // --------------------
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    LOG << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwSetScrollCallback(window, mouse_scroll_callback);

  // Capture mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // stbi_set_flip_vertically_on_load(true);

  // Configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // Build and compile shaders
  // -------------------------
  Shader shader_red("../shader/box_red.vs", "../shader/box.fs");
  Shader shader_green("../shader/box_green.vs", "../shader/box.fs");
  Shader shader_blue("../shader/box_blue.vs", "../shader/box.fs");
  Shader shader_yellow("../shader/box_yellow.vs", "../shader/box.fs");
  // Objects
  // -------
  // Cube objects
  GLuint cube_VAO, cube_VBO;
  glGenVertexArrays(1, &cube_VAO);
  glGenBuffers(1, &cube_VBO);
  glBindVertexArray(cube_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vert_pos), &cube_vert_pos,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * FLOAT_SIZE, (void *)0);
  glBindVertexArray(0);

  // Texture
  // -------

  // Uniform block
  GLuint uniform_block_idx_red =
      glGetUniformBlockIndex(shader_red.ID, "Matrices");
  GLuint uniform_block_idx_green =
      glGetUniformBlockIndex(shader_green.ID, "Matrices");
  GLuint uniform_block_idx_blue =
      glGetUniformBlockIndex(shader_blue.ID, "Matrices");
  GLuint uniform_block_idx_yellow =
      glGetUniformBlockIndex(shader_yellow.ID, "Matrices");

  glUniformBlockBinding(shader_red.ID, uniform_block_idx_red, 0);
  glUniformBlockBinding(shader_green.ID, uniform_block_idx_green, 0);
  glUniformBlockBinding(shader_blue.ID, uniform_block_idx_blue, 0);
  glUniformBlockBinding(shader_yellow.ID, uniform_block_idx_yellow, 0);

  GLuint ubo_matrices;
  glGenBuffers(1, &ubo_matrices);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_matrices, 0,
                    2 * sizeof(glm::mat4));

  // Fill
  glm::mat4 projection = glm::perspective(
      45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(projection),
                  glm::value_ptr(projection));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // Render loop
  // -----------
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while (!glfwWindowShouldClose(window)) {
    // Per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - last_frame;
    last_frame = currentFrame;

    // Input
    // -----
    process_input(window);
    glfwPollEvents();

    // Render
    // ------
    glClearColor(0.3f, 0.3f, 0.3f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw models on custom buffer
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.viewMatrix();
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(view),
                    glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // Cubes
    glBindVertexArray(cube_VAO);
    // R
    shader_red.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f));
    shader_red.setMat4fv("model", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // G
    shader_green.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f));
    shader_green.setMat4fv("model", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // B
    shader_blue.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f));
    shader_blue.setMat4fv("model", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // Y
    shader_yellow.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f));
    shader_yellow.setMat4fv("model", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

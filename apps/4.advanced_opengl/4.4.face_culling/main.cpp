#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learn-opengl/Shader.h"
#include "learn-opengl/Camera.h"
#include "learn-opengl/image.h"

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
float cube_vertices[] = {
  // Back face
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
  // Front face
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
  // Left face
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
  // Right face
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
  // Bottom face
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
  // Top face
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left
};
// Note we set tex coords higher than 1
// Together with GL_REPEAT as texture wrapping mode
// This will cause the floor texture to repeat
float plane_vertices[] = {
  // positions          // texture Coords
   5.0f, -0.51f,  5.0f,  2.0f, 0.0f,
  -5.0f, -0.51f, -5.0f,  0.0f, 2.0f,
  -5.0f, -0.51f,  5.0f,  0.0f, 0.0f,

   5.0f, -0.51f,  5.0f,  2.0f, 0.0f,
   5.0f, -0.51f, -5.0f,  2.0f, 2.0f,
  -5.0f, -0.51f, -5.0f,  0.0f, 2.0f
};
// clang-format on
void framebuffer_size_callback(GLFWwindow *, int w, int h) {
  glViewport(0, 0, w, h);
}
void process_input(GLFWwindow *window) {
  // Close window on Esc pressed
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  // Camera move
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
void mouse_move_callback(GLFWwindow *, double xpos, double ypos) {
  camera.turn(xpos, ypos);
}
void mouse_scroll_callback(GLFWwindow *, double , double yoffset) {
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
    std::cout << "Failed to create GLFW window" << std::endl;
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


  // Configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // Build and compile shaders
  // -------------------------
  Shader model_shader("../shader/model.vs", "../shader/model.fs");

  // Cube objects
  GLuint cube_VAO, cube_VBO;
  glGenVertexArrays(1, &cube_VAO);
  glGenBuffers(1, &cube_VBO);
  glBindVertexArray(cube_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);
  // Plane objects
  GLuint plane_VAO, plane_VBO;
  glGenVertexArrays(1, &plane_VAO);
  glGenBuffers(1, &plane_VBO);
  glBindVertexArray(plane_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, plane_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);
  // Texture
  GLuint cube_texture = loadTexture("../texture/marble.jpg");
  GLuint floor_texture = loadTexture("../texture/metal.png");

  // Render loop
  // -----------
  model_shader.use();
  model_shader.setInt("texture1", 0);
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
    glClearColor(0.1, 0.1, 0.1, 1.0f);

    // Use model shader
    model_shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.viewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.fov()), 1.0 * SCR_WIDTH / SCR_HEIGHT, 0.1, 100.0);
    model_shader.setMat4fv("view", glm::value_ptr(view));
    model_shader.setMat4fv("projection", glm::value_ptr(projection));

    // Normal rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Floor
    glBindTexture(GL_TEXTURE_2D, floor_texture);
    model_shader.setMat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    glBindVertexArray(plane_VAO);
    glStencilMask(0x00);  // Forbid stencil updating of floor part
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Cubes
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cube_texture);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, -0.5f, -1.0f));
    model_shader.setMat4fv("model", glm::value_ptr(model));
    glBindVertexArray(cube_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    model_shader.setMat4fv("model", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

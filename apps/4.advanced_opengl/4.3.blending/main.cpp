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
float cube_vertices[] = {
  // positions          // texture Coords
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
// Note we set tex coords higher than 1
// Together with GL_REPEAT as texture wrapping mode
// This will cause the floor texture to repeat
float plane_vertices[] = {
  // positions          // texture Coords
   5.0f, -0.51f,  5.0f,  2.0f, 0.0f,
  -5.0f, -0.51f,  5.0f,  0.0f, 0.0f,
  -5.0f, -0.51f, -5.0f,  0.0f, 2.0f,

   5.0f, -0.51f,  5.0f,  2.0f, 0.0f,
  -5.0f, -0.51f, -5.0f,  0.0f, 2.0f,
   5.0f, -0.51f, -5.0f,  2.0f, 2.0f
};
float blend_vertices[] = {
// Swapped y coordinates because texture is flipped upside down
  // positions         // texture Coords
  0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
  0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
  1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

  0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
  1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
  1.0f,  0.5f,  0.0f,  1.0f,  0.0f
};
// Grass position
std::vector<glm::vec3> blend_pos {
  glm::vec3(-1.5f,  0.0f, -0.48f),
  glm::vec3( 1.5f,  0.1f,  0.51f),
  glm::vec3( 0.0f,  0.2f,  0.7f),
  glm::vec3(-0.3f,  0.3f, -2.3f),
  glm::vec3( 0.5f,  0.4f, -0.6f)
};
// Blend problem
// These three triangles are overlapped on each other
float blend_prob[] = {
  // position       // color
  // Triangle 1, red
  0.00f,  0.00f, -4.00f, 1.0, 0.0, 0.0, 0.3,
  0.30f,  0.00f, -4.00f, 1.0, 0.0, 0.0, 0.3,
  0.50f,  0.86f, -4.10f, 1.0, 0.0, 0.0, 0.3,
  // Triangle 2, green
  0.40f,  0.86f, -4.00f, 0.0, 1.0, 0.0, 0.3,
  0.25f,  0.61f, -4.00f, 0.0, 1.0, 0.0, 0.3,
  0.90f,  0.00f, -4.10f, 0.0, 1.0, 0.0, 0.3,
  // Triangle 3, blue
  1.00f,  0.10f, -4.00f, 0.0, 0.0, 1.0, 0.3,
  0.85f,  0.36f, -4.00f, 0.0, 0.0, 1.0, 0.3,
  0.00f,  0.10f, -4.10f, 0.0, 0.0, 1.0, 0.3,
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

  // stbi_set_flip_vertically_on_load(true);

  // Configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Build and compile shaders
  // -------------------------
  Shader model_shader("../shader/model.vs", "../shader/model.fs");
  Shader blend_shader("../shader/blend.vs", "../shader/blend.fs");

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
  // Blend objects
  GLuint blend_VAO, blend_VBO;
  glGenBuffers(1, &blend_VBO);
  glGenVertexArrays(1, &blend_VAO);
  glBindVertexArray(blend_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, blend_VAO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(blend_vertices), &blend_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);
  // Blend problem
  GLuint prob_VAO, prob_VBO;
  glGenBuffers(1, &prob_VBO);
  glGenVertexArrays(1, &prob_VAO);
  glBindVertexArray(prob_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, prob_VAO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(blend_prob), &blend_prob,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);
  // Texture
  GLuint cube_texture = load_texture("../texture/marble.jpg");
  GLuint floor_texture = load_texture("../texture/metal.png");
  GLuint blend_texture =
      load_texture("../texture/window.png", GL_CLAMP_TO_BORDER);

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

    // Blend
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blend_texture);
    // Sort the transparent objects by descending distance,
    // for correct blending with depth testing enabled.
    std::map<float, glm::vec3> blend_pos_sorted;
    for (auto &pos : blend_pos) {
      float dis = glm::length(camera.cameraPosition() - pos);
      blend_pos_sorted[-dis] = pos;
    }
    for (auto &dis_pos : blend_pos_sorted) {
      model = glm::mat4(1.0);
      model = glm::translate(model, dis_pos.second);
      model_shader.setMat4fv("model", glm::value_ptr(model));
      glBindVertexArray(blend_VAO);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    // Blend problem
    blend_shader.use();
    blend_shader.setMat4fv("projection", glm::value_ptr(projection));
    blend_shader.setMat4fv("view", glm::value_ptr(view));
    model = glm::mat4(1.0);
    blend_shader.setMat4fv("model", glm::value_ptr(model));
    glBindVertexArray(prob_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawArrays(GL_TRIANGLES, 3, 3);
    glDrawArrays(GL_TRIANGLES, 6, 3);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

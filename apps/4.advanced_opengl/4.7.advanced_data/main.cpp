#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learn-opengl/Shader.h"
#include "learn-opengl/Camera.h"
#include "learn-opengl/gl_utility.h"
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
float cube_vert_tex[] = {
  // Back face
   0.0f, 0.0f, // Bottom-left
   1.0f, 1.0f, // top-right
   1.0f, 0.0f, // bottom-right
   1.0f, 1.0f, // top-right
   0.0f, 0.0f, // bottom-left
   0.0f, 1.0f, // top-left
  // Front face
   0.0f, 0.0f, // bottom-left
   1.0f, 0.0f, // bottom-right
   1.0f, 1.0f, // top-right
   1.0f, 1.0f, // top-right
   0.0f, 1.0f, // top-left
   0.0f, 0.0f, // bottom-left
  // Left face
   1.0f, 0.0f, // top-right
   1.0f, 1.0f, // top-left
   0.0f, 1.0f, // bottom-left
   0.0f, 1.0f, // bottom-left
   0.0f, 0.0f, // bottom-right
   1.0f, 0.0f, // top-right
  // Right face
   1.0f, 0.0f, // top-left
   0.0f, 1.0f, // bottom-right
   1.0f, 1.0f, // top-right
   0.0f, 1.0f, // bottom-right
   1.0f, 0.0f, // top-left
   0.0f, 0.0f, // bottom-left
  // Bottom face
   0.0f, 1.0f, // top-right
   1.0f, 1.0f, // top-left
   1.0f, 0.0f, // bottom-left
   1.0f, 0.0f, // bottom-left
   0.0f, 0.0f, // bottom-right
   0.0f, 1.0f, // top-right
  // Top face
   0.0f, 1.0f, // top-left
   1.0f, 0.0f, // bottom-right
   1.0f, 1.0f, // top-right
   1.0f, 0.0f, // bottom-right
   0.0f, 1.0f, // top-left
   0.0f, 0.0f // bottom-left
};
float cube_vert_normal[] = {
  // Back face
   0.0f,  0.0f, -1.0f, // Bottom-left
   0.0f,  0.0f, -1.0f, // top-right
   0.0f,  0.0f, -1.0f, // bottom-right
   0.0f,  0.0f, -1.0f, // top-right
   0.0f,  0.0f, -1.0f, // bottom-left
   0.0f,  0.0f, -1.0f, // top-left
  // Front face
   0.0f,  0.0f,  1.0f, // bottom-left
   0.0f,  0.0f,  1.0f, // bottom-right
   0.0f,  0.0f,  1.0f, // top-right
   0.0f,  0.0f,  1.0f, // top-right
   0.0f,  0.0f,  1.0f, // top-left
   0.0f,  0.0f,  1.0f, // bottom-left
  // Left face
  -1.0f,  0.0f,  0.0f, // top-right
  -1.0f,  0.0f,  0.0f, // top-left
  -1.0f,  0.0f,  0.0f, // bottom-left
  -1.0f,  0.0f,  0.0f, // bottom-left
  -1.0f,  0.0f,  0.0f, // bottom-right
  -1.0f,  0.0f,  0.0f, // top-right
  // Right face
   1.0f,  0.0f,  0.0f, // top-left
   1.0f,  0.0f,  0.0f, // bottom-right
   1.0f,  0.0f,  0.0f, // top-right
   1.0f,  0.0f,  0.0f, // bottom-right
   1.0f,  0.0f,  0.0f, // top-left
   1.0f,  0.0f,  0.0f, // bottom-left
  // Bottom face
   0.0f, -1.0f,  0.0f, // top-right
   0.0f, -1.0f,  0.0f, // top-left
   0.0f, -1.0f,  0.0f, // bottom-left
   0.0f, -1.0f,  0.0f, // bottom-left
   0.0f, -1.0f,  0.0f, // bottom-right
   0.0f, -1.0f,  0.0f, // top-right
  // Top face
   0.0f,  1.0f,  0.0f, // top-left
   0.0f,  1.0f,  0.0f, // bottom-right
   0.0f,  1.0f,  0.0f, // top-right
   0.0f,  1.0f,  0.0f, // bottom-right
   0.0f,  1.0f,  0.0f, // top-left
   0.0f,  1.0f,  0.0f  // bottom-left
};
float skybox_vert_pos[] = {
  -1.0f,  1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

  -1.0f,  1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f
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


  // Configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // Build and compile shaders
  // -------------------------
  Shader model_shader("../shader/model.vs", "../shader/model.fs");
  Shader skybox_shader("../shader/cubemap.vs", "../shader/cubemap.fs");
  // Objects
  // -------
  // Cube objects
  GLuint cube_VAO, cube_VBO;
  glGenVertexArrays(1, &cube_VAO);
  glGenBuffers(1, &cube_VBO);
  glBindVertexArray(cube_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);

  glBufferData(
      GL_ARRAY_BUFFER,
      sizeof(cube_vert_pos) + sizeof(cube_vert_tex) + sizeof(cube_vert_normal),
      NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cube_vert_pos), &cube_vert_pos);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(cube_vert_pos), sizeof(cube_vert_tex),
                  &cube_vert_tex);
  glBufferSubData(GL_ARRAY_BUFFER,
                  sizeof(cube_vert_pos) + sizeof(cube_vert_tex),
                  sizeof(cube_vert_normal), &cube_vert_normal);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * FLOAT_SIZE, (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * FLOAT_SIZE,
                        (void *)(sizeof(cube_vert_pos)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(
      2, 3, GL_FLOAT, GL_FALSE, 3 * FLOAT_SIZE,
      (void *)(sizeof(cube_vert_pos) + sizeof(cube_vert_tex)));

  glBindVertexArray(0);
  // Skybox
  GLuint skybox_VAO, skybox_VBO;
  glGenVertexArrays(1, &skybox_VAO);
  glGenBuffers(1, &skybox_VBO);
  glBindVertexArray(skybox_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vert_pos), &skybox_vert_pos,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * FLOAT_SIZE, (void *)0);
  glBindVertexArray(0);

  // Texture
  // -------
  GLuint model_diffuse = loadTexture("../texture/container2_diffuse.png");
  GLuint model_specular = loadTexture("../texture/container2_specular.png");
  // Cubemap texture
  std::vector<std::string> cube_faces{
      "../texture/skybox/right.jpg", "../texture/skybox/left.jpg",
      "../texture/skybox/top.jpg",   "../texture/skybox/bottom.jpg",
      "../texture/skybox/front.jpg", "../texture/skybox/back.jpg"};
  GLuint cubemap = loadCubemap(cube_faces);

  model_shader.use();
  model_shader.setInt("texture_diffuse", 0);
  model_shader.setInt("texture_specular", 1);
  model_shader.setInt("skybox", 2);
  skybox_shader.use();
  skybox_shader.setInt("skybox", 0);

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
    glClearColor(0.1f, 0.1f, 0.1f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw models on custom buffer
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.viewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(float(camera.fov())),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    // Cubes
    model_shader.use();
    model_shader.setMat4fv("view", glm::value_ptr(view));
    model_shader.setMat4fv("projection", glm::value_ptr(projection));
    model_shader.setVec3fv("camera_pos",
                            glm::value_ptr(camera.cameraPosition()));
    glBindVertexArray(cube_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model_diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, model_specular);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, -1.2f));
    model_shader.setMat4fv("model", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.2f, -0.2f, 0.0f));
    model_shader.setMat4fv("model", glm::value_ptr(model));
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // Skybox
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(skybox_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    skybox_shader.use();
    skybox_shader.setMat4fv(
        "view", glm::value_ptr(glm::mat4(glm::mat3(camera.viewMatrix()))));
    skybox_shader.setMat4fv("projection", glm::value_ptr(projection));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

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
float quad_vertices[] = {
  // positions   // texCoords
  -1.0f,  1.0f,  0.0f, 1.0f,
  -1.0f, -1.0f,  0.0f, 0.0f,
   1.0f, -1.0f,  1.0f, 0.0f,

  -1.0f,  1.0f,  0.0f, 1.0f,
   1.0f, -1.0f,  1.0f, 0.0f,
   1.0f,  1.0f,  1.0f, 1.0f
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
  Shader screen_shader("../shader/screen.vs", "../shader/screen.fs");
  Shader model_shader("../shader/model.vs", "../shader/model.fs");

  // Objects
  // -------
  // Cube objects
  GLuint cube_VAO, cube_VBO;
  glGenVertexArrays(1, &cube_VAO);
  glGenBuffers(1, &cube_VBO);
  glBindVertexArray(cube_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * FLOAT_SIZE, (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * FLOAT_SIZE,
                        (void *)(3 * FLOAT_SIZE));
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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * FLOAT_SIZE, (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * FLOAT_SIZE,
                        (void *)(3 * FLOAT_SIZE));
  glBindVertexArray(0);
  // Quad objects
  GLuint quad_VAO, quad_VBO;
  glGenVertexArrays(1, &quad_VAO);
  glGenBuffers(1, &quad_VBO);
  glBindVertexArray(quad_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, quad_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * FLOAT_SIZE, (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * FLOAT_SIZE,
                        (void *)(2 * FLOAT_SIZE));
  glBindVertexArray(0);

  // Texture
  // -------
  GLuint cube_texture = load_texture("../texture/container.jpg");
  GLuint floor_texture = load_texture("../texture/metal.png");

  screen_shader.use();
  screen_shader.setInt("texture1", 0);
  model_shader.use();
  model_shader.setInt("texture1", 0);

  // Custom framebuffer
  // ------------------
  // Color buffer
  GLuint frame_buffer;
  glGenFramebuffers(1, &frame_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
  // Generate texture for attachment
  GLuint tex_colorbuffer;
  glGenTextures(1, &tex_colorbuffer);
  glBindTexture(GL_TEXTURE_2D, tex_colorbuffer);
  glTexImage2D(GL_TEXTURE_2D,     // Target
               0,                 // Level
               GL_RGB,            // Internal format for OpenGL storage
               SCR_WIDTH, SCR_HEIGHT,          // Width and height
               0,                 // Border (always zero)
               GL_RGB,            // Data format layout
               GL_UNSIGNED_BYTE,  // Data format type
               NULL               // Pointer to texture data
  );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  // Attach
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         tex_colorbuffer, 0);
  // Depth and stencil buffer
  GLuint render_buffer;
  glGenRenderbuffers(1, &render_buffer);
  glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
  // Attach
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  // Check framebuffers
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    LOG << "::main(): Framebuffer is not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
    // Render on custom buffer
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw models on custom buffer
    model_shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.viewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(float(camera.fov())),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    model_shader.setMat4fv("view", glm::value_ptr(view));
    model_shader.setMat4fv("projection", glm::value_ptr(projection));
    // Cubes
    glBindVertexArray(cube_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cube_texture);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    model_shader.setMat4fv("model", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, -0.2f, 0.0f));
    model_shader.setMat4fv("model", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // Floor
    glBindVertexArray(plane_VAO);
    glBindTexture(GL_TEXTURE_2D, floor_texture);
    model_shader.setMat4fv("model", glm::value_ptr(glm::mat4(1.0f)));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Render on default buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);  // Only a quad, no depth required.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    screen_shader.use();
    glBindVertexArray(quad_VAO);
    glBindTexture(GL_TEXTURE_2D, tex_colorbuffer);
    // Sample from custom color buffer
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

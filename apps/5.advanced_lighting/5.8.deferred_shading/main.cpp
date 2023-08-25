#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "ImGui/backend/imgui_impl_glfw.h"
#include "ImGui/backend/imgui_impl_opengl3.h"
#include "ImGui/imgui.h"
//
#include "learn-opengl/Camera.h"
#include "learn-opengl/Model.h"
#include "learn-opengl/Shader.h"
#include "learn-opengl/gl_utility.h"
#include "learn-opengl/image.h"

constexpr int SCREEN_W = 1024;
constexpr int SCREEN_H = 768;

glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 5.0f);
// Front direction, not looking at point
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
Camera camera{camPos, camFront, camUp};

glm::vec3 lightPos(0.0f, 0.0f, 2.0f);
// Exposure.
float exposure = 0.1;

float deltaTime = 0;
float lastFrame = 0;

// Data
void framebuffer_size_callback(GLFWwindow *, int w, int h);
void process_input(GLFWwindow *window);
void mouse_move_callback(GLFWwindow *, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow *, double, double yoffset);
void renderScene(const Shader &shader);
void renderCube();
void renderQuad();

int main() {
  /// @brief init glfw window
  glfwInit();                                    // init GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.x
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL x.3
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE); // use core profile
  // Get the glfw window
  GLFWwindow *window =
      glfwCreateWindow(SCREEN_W, SCREEN_H, "LearnOpenGL", NULL, NULL);
  if (window == NULL) { // check
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  // Mouse input
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwSetScrollCallback(window, mouse_scroll_callback);
  // Load all OpenGL function pointers through glad.
  if (!gladLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  /**************************** Render init ****************************/
  glEnable(GL_DEPTH_TEST);

  // Setup Dear ImGui context
  printf("imgui context\n");
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  printf("imgui binding\n");
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char *glsl_version = "#version 330";
  ImGui_ImplOpenGL3_Init(glsl_version);

  glViewport(0, 0, SCREEN_W, SCREEN_H);
  // Resize callback.
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  /// @brief Build shader program
  Shader shaderGeometry{"../shaders/gpass.vert", "../shaders/gpass.frag"};
  Shader shaderLighting{"../shaders/lpass.vert", "../shaders/lpass.frag"};
  Shader shaderTexture{"../shaders/texture.vert", "../shaders/texture.frag"};
  shaderLighting.use();
  shaderLighting.setInt("gPosition", 0);
  shaderLighting.setInt("gNormal", 1);
  shaderLighting.setInt("gDiffSpec", 2);
  shaderTexture.use();
  shaderTexture.setInt("texture1", 0);

  /// @brief Other data.
  Model backpack("../models/backpack/backpack.obj");

  std::vector<glm::vec3> objectPositions;
  objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
  objectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
  objectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
  objectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
  objectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
  objectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
  objectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
  objectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
  objectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));
  constexpr unsigned int N_LIGHTS = 32;
  std::vector<glm::vec3> lightPositions;
  std::vector<glm::vec3> lightColors;
  srand(time(nullptr));
  for (unsigned int i = 0; i < N_LIGHTS; i++) {
    float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
    float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
    float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
    lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
    float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5);
    float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5);
    float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5);
    lightColors.push_back(glm::vec3(rColor, gColor, bColor));
  }

  /// @brief Other ops.
  // Prepare g-buffers using color buffer.
  GLuint bufferG = 0;
  glGenFramebuffers(1, &bufferG);
  glBindFramebuffer(GL_FRAMEBUFFER, bufferG);
  GLuint gBufferTextures[3] = {0};
  GLenum gBufferTexInternalFmt[3] = {GL_RGBA16F, GL_RGBA16F, GL_RGBA};
  GLenum gBufferTexDataType[3] = {GL_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE};
  GLenum gBufferTexAttach[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                GL_COLOR_ATTACHMENT2};
  for (int i = 0; i < 3; i++) {
    glGenTextures(1, &(gBufferTextures[i]));
    glBindTexture(GL_TEXTURE_2D, gBufferTextures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, gBufferTexInternalFmt[i], SCREEN_W, SCREEN_H,
                 0, GL_RGBA, gBufferTexDataType[i], NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, gBufferTexAttach[i], GL_TEXTURE_2D,
                           gBufferTextures[i], 0);
  }
  glDrawBuffers(3, gBufferTexAttach);
  GLuint gBufferDepth;
  glGenRenderbuffers(1, &gBufferDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, gBufferDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_W,
                        SCREEN_H);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, gBufferDepth);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /// @brief Render loop
  while (!glfwWindowShouldClose(window)) {
    /**************************** Input ****************************/
    process_input(window);

    /**************************** Logic ****************************/
    glfwPollEvents();
    float curTime = glfwGetTime();
    deltaTime = curTime - lastFrame;
    lastFrame = curTime;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
      ImGui::Begin("Deferred shading");
      ImGui::Text("%.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::Text("Camera position: (%.2f, %.2f, %.2f)",
                  camera.cameraPosition().x, camera.cameraPosition().y,
                  camera.cameraPosition().z);
      ImGui::Text("Camera front: (%.2f, %.2f, %.2f)", camera.cameraFront().x,
                  camera.cameraFront().y, camera.cameraFront().z);
      ImGui::Text("Camera up: (%.2f, %.2f, %.2f)", camera.cameraUp().x,
                  camera.cameraUp().y, camera.cameraUp().z);
      ImGui::Text("Camera right: (%.2f, %.2f, %.2f)", camera.cameraRight().x,
                  camera.cameraRight().y, camera.cameraRight().z);

      ImGui::Text("Exposure: %.2f", exposure);

      ImGui::End();
    }

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /**************************** Rendering ****************************/
    /// @brief Geometry pass.
    glBindFramebuffer(GL_FRAMEBUFFER, bufferG);
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glm::mat4 projection = glm::perspective(
          glm::radians(camera.fov()), 1.0 * SCREEN_W / SCREEN_H, 0.1, 100.0);
      glm::mat4 view = camera.viewMatrix();
      glm::mat4 model = glm::mat4{1};
      shaderGeometry.use();
      shaderGeometry.setBool("inverse_normal", false);
      shaderGeometry.setMat4fv("projection", glm::value_ptr(projection));
      shaderGeometry.setMat4fv("view", glm::value_ptr(view));
      for (size_t i = 0; i < objectPositions.size(); i++) {
        // Object-depended uniforms and rendering.
        model = glm::mat4{1};
        model = glm::translate(model, objectPositions[i]);
        model = glm::scale(model, glm::vec3{0.5, 0.5, 0.5});
        shaderGeometry.setMat4fv("model", glm::value_ptr(model));
        backpack.draw(shaderGeometry);
      }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /// @brief Lighting pass
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderLighting.use();
    // Bind all G-buffer textures.
    for (unsigned int i = 0; i < 3; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, gBufferTextures[i]);
    }
    // Lighting uniforms.
    for (size_t i = 0; i < lightPositions.size(); i++) {
      shaderLighting.setVec3fv(
          ("lights[" + std::to_string(i) + "].Position").c_str(),
          glm::value_ptr(lightPositions[i]));
      shaderLighting.setVec3fv(
          ("lights[" + std::to_string(i) + "].Color").c_str(),
          glm::value_ptr(lightColors[i]));
      constexpr float linear = 0.7f;
      constexpr float quadratic = 1.8f;
      shaderLighting.setFloat(
          ("lights[" + std::to_string(i) + "].aLinear").c_str(), linear);
      shaderLighting.setFloat(
          ("lights[" + std::to_string(i) + "].aQuadratic").c_str(), quadratic);
    }
    shaderLighting.setVec3fv("viewPos",
                             glm::value_ptr(camera.cameraPosition()));
    shaderLighting.setFloat("exposure", exposure);
    renderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /// @brief Debugging.
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // shaderTexture.use();
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, gBufferTextures[2]);
    // renderQuad();
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (glCheckError() != GL_NO_ERROR)
      break;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  printf("clean\n");
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
  std::cout << "done\n";
  return 0;
}

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

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    exposure = exposure + 0.01 > 10 ? 10 : exposure + 0.01;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    exposure = exposure - 0.01 < 0 ? 0 : exposure - 0.01;
}
double lastX, lastY;
bool firstMouse = true;
void mouse_move_callback(GLFWwindow *, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  // reversed since y-coordinates go from bottom to top
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;
  camera.turn(xoffset, yoffset);
}

void mouse_scroll_callback(GLFWwindow *, double, double yoffset) {
  camera.zoom(yoffset);
}
// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube() {
  if (cubeVAO == 0) {
    float vertices[] = {
        // clang-format off
      // back face
      -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
       1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
       1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
       1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
      -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
      -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
      // front face
      -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
       1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
       1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
       1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
      -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
      -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
      // left face
      -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
      -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
      -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
      -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
      -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
      -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
      // right face
       1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
       1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
       1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
       1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
       1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
       1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
      // bottom face
      -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
       1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
       1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
       1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
      -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
      -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
      // top face
      -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
       1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
       1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
       1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
      -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
      -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        // clang-format on
    };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * FLOAT_SIZE, (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * FLOAT_SIZE,
                          (void *)(3 * FLOAT_SIZE));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * FLOAT_SIZE,
                          (void *)(6 * FLOAT_SIZE));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  // render Cube
  glBindVertexArray(cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
  if (quadVAO == 0) {
    float quadVertices[] = {
        // clang-format off
      // positions        // texture Coords
      -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        // clang-format on
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * FLOAT_SIZE, (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * FLOAT_SIZE,
                          (void *)(3 * FLOAT_SIZE));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

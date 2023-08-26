#include <glad/glad.h>
#include <random>
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
// #include "learn-opengl/image.h"

constexpr int SCREEN_W = 1024;
constexpr int SCREEN_H = 768;

glm::vec3 camPos = glm::vec3(0.0f, 0.0f, -5.0f);
// Front direction, not looking at point
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
Camera camera{camPos, camFront, camUp};

glm::vec3 lightPos(0.0f, 0.0f, 2.0f);
// Exposure.
float exposure = 0.1;
// SSAO switch.
int ssao = 0;

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
  Shader shaderLighting{"../shaders/texture.vert", "../shaders/lpass.frag"};
  Shader shaderLightBox{"../shaders/lightbox.vert", "../shaders/lightbox.frag"};
  Shader shaderHDR{"../shaders/texture.vert", "../shaders/hdr.frag"};
  Shader shaderSSAO{"../shaders/texture.vert", "../shaders/ssao.frag"};
  Shader shaderSSAOBlur{"../shaders/texture.vert", "../shaders/blur.frag"};
  Shader shaderTexture{"../shaders/texture.vert", "../shaders/texture.frag"};
  shaderLighting.use();
  shaderLighting.setInt("gPosition", 0);
  shaderLighting.setInt("gNormal", 1);
  shaderLighting.setInt("gDiffSpec", 2);
  shaderLighting.setInt("texSSAO", 3);
  shaderHDR.use();
  shaderHDR.setInt("texture1", 0);
  shaderSSAO.use();
  shaderSSAO.setInt("gPosition", 0);
  shaderSSAO.setInt("gNormal", 1);
  shaderSSAO.setInt("texNoise", 2);
  shaderSSAOBlur.use();
  shaderSSAOBlur.setInt("texSSAO", 0);
  shaderTexture.use();
  shaderTexture.setInt("texture1", 0);

  /// @brief Other data.
  Model backpack("../models/backpack/backpack.obj");

  // Lights.
  // constexpr unsigned int N_LIGHTS = 1;
  std::vector<glm::vec3> lightPositions = {glm::vec3{0, 5, 0}};
  std::vector<glm::vec3> lightColors = {glm::vec3{10, 10, 10}};
  // SSAO.
  std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
  std::default_random_engine generator;
  std::vector<glm::vec3> ssaoKernel;
  for (int i = 0; i < 64; ++i) {
    // Uniform distribution in hemishpere along +z axis.
    float e1 = randomFloats(generator) * glm::pi<float>() * 0.5;
    float e2 = randomFloats(generator) * 2.0 - 1.0 * glm::pi<float>();
    float e3 = sqrt(randomFloats(generator));
    glm::vec3 sample(sin(e1) * cos(e2) * e3, sin(e1) * sin(e2) * e3,
                     cos(e1) * e3);
    // Shift closer to the origin.
    float scale = 1.0 * i / 64;
    scale = 0.1 + 0.9 * scale * scale;
    sample *= scale;
    ssaoKernel.push_back(sample);
  }
  // Random rotation, stored in a 4x4 texture to be tiled.
  // Will apply on the tangent basis.
  std::vector<glm::vec3> ssaoNoise;
  for (unsigned int i = 0; i < 16; i++) {
    glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator) * 2.0 - 1.0, 0.0f);
    ssaoNoise.push_back(noise);
  }

  /// @brief Other ops.
  GLuint texNoise;
  glGenTextures(1, &texNoise);
  glBindTexture(GL_TEXTURE_2D, texNoise);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT,
               &ssaoNoise[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // G-buffer.
  GLuint fboG = 0;
  glGenFramebuffers(1, &fboG);
  glBindFramebuffer(GL_FRAMEBUFFER, fboG);
  // Position, Normal, DiffSpec.
  GLuint texGColor[3] = {0};
  // Diffuse and specular is ok in low precision.
  GLenum texGColorInternalFmt[3] = {GL_RGBA16F, GL_RGBA16F, GL_RGBA};
  GLenum texGColorDataType[3] = {GL_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE};
  GLenum texGColorAttach[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                               GL_COLOR_ATTACHMENT2};
  for (int i = 0; i < 3; i++) {
    glGenTextures(1, &(texGColor[i]));
    glBindTexture(GL_TEXTURE_2D, texGColor[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, texGColorInternalFmt[i], SCREEN_W, SCREEN_H,
                 0, GL_RGBA, texGColorDataType[i], NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, texGColorAttach[i], GL_TEXTURE_2D,
                           texGColor[i], 0);
  }
  glDrawBuffers(3, texGColorAttach);
  GLuint rboGDepth;
  glGenRenderbuffers(1, &rboGDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboGDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_W,
                        SCREEN_H);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboGDepth);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // HDR buffer.
  GLuint fboHDR = 0;
  glGenFramebuffers(1, &fboHDR);
  glBindFramebuffer(GL_FRAMEBUFFER, fboHDR);
  GLuint texHDRColor = 0;
  glGenTextures(1, &texHDRColor);
  glBindTexture(GL_TEXTURE_2D, texHDRColor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_W, SCREEN_H, 0, GL_RGBA,
               GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texHDRColor, 0);
  GLuint rboHDRDepth = 0;
  glGenRenderbuffers(1, &rboHDRDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboHDRDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_W,
                        SCREEN_H);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboHDRDepth);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // SSAO buffer.
  GLuint fboSSAO;
  glGenFramebuffers(1, &fboSSAO);
  glBindFramebuffer(GL_FRAMEBUFFER, fboSSAO);
  GLuint texSSAOColor;
  glGenTextures(1, &texSSAOColor);
  glBindTexture(GL_TEXTURE_2D, texSSAOColor);
  // Single float data.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCREEN_W, SCREEN_H, 0, GL_RED,
               GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texSSAOColor, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // SSAO blur buffer.
  GLuint fboSSAOBlur, texSSAOBlurColor;
  glGenFramebuffers(1, &fboSSAOBlur);
  glBindFramebuffer(GL_FRAMEBUFFER, fboSSAOBlur);
  glGenTextures(1, &texSSAOBlurColor);
  glBindTexture(GL_TEXTURE_2D, texSSAOBlurColor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCREEN_W, SCREEN_H, 0, GL_RED,
               GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texSSAOBlurColor, 0);
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
      ImGui::Begin("SSAO");
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
      ImGui::Text("SSAO: %d", ssao);

      ImGui::End();
    }

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /**************************** Rendering ****************************/
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.fov()), 1.0 * SCREEN_W / SCREEN_H, 0.1, 100.0);
    glm::mat4 view = camera.viewMatrix();
    glm::mat4 model = glm::mat4{1};
    /// @brief Geometry pass.
    glBindFramebuffer(GL_FRAMEBUFFER, fboG);
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      shaderGeometry.use();
      shaderGeometry.setBool("inverse_normal", false);
      shaderGeometry.setMat4fv("projection", glm::value_ptr(projection));
      shaderGeometry.setMat4fv("view", glm::value_ptr(view));

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
      model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
      shaderGeometry.setMat4fv("model", glm::value_ptr(model));
      shaderGeometry.setBool("inverse_normal", true);
      renderCube();
      // backpack model on the floor
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0));
      model =
          glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
      model = glm::scale(model, glm::vec3(1.0f));
      shaderGeometry.setMat4fv("model", glm::value_ptr(model));
      shaderGeometry.setBool("inverse_normal", false);
      backpack.draw(shaderGeometry);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /// @brief SSAO pass.
    glBindFramebuffer(GL_FRAMEBUFFER, fboSSAO);
    {
      glClear(GL_COLOR_BUFFER_BIT);
      shaderSSAO.use();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texGColor[0]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texGColor[1]);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, texNoise);
      // Send sample vectors.
      for (int i = 0; i < 64; i++) {
        std::string name = "samples[" + std::to_string(i) + "]";
        shaderSSAO.setVec3fv(name.c_str(), glm::value_ptr(ssaoKernel[i]));
      }
      shaderSSAO.setMat4fv("projection", glm::value_ptr(projection));
      renderQuad();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // SSAO blur.
    glBindFramebuffer(GL_FRAMEBUFFER, fboSSAOBlur);
    {
      glClear(GL_COLOR_BUFFER_BIT);
      shaderSSAOBlur.use();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texSSAOColor);
      renderQuad();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /// @brief Lighting pass.
    glBindFramebuffer(GL_FRAMEBUFFER, fboHDR);
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      shaderLighting.use();
      // Bind all G-buffer textures.
      for (unsigned int i = 0; i < 3; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texGColor[i]);
      }
      glActiveTexture(GL_TEXTURE0 + 3);
      glBindTexture(GL_TEXTURE_2D, texSSAOBlurColor);
      // Lighting uniforms.
      for (size_t i = 0; i < lightPositions.size(); i++) {
        shaderLighting.setVec3fv(
            ("lights[" + std::to_string(i) + "].Position").c_str(),
            glm::value_ptr(lightPositions[i]));
        shaderLighting.setVec3fv(
            ("lights[" + std::to_string(i) + "].Color").c_str(),
            glm::value_ptr(lightColors[i]));
        constexpr float linear = 0.2f;
        constexpr float quadratic = 1.0f;
        shaderLighting.setFloat(
            ("lights[" + std::to_string(i) + "].aLinear").c_str(), linear);
        shaderLighting.setFloat(
            ("lights[" + std::to_string(i) + "].aQuadratic").c_str(),
            quadratic);
      }
      shaderLighting.setBool("ssao", bool(ssao));
      renderQuad();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Light boxes.
    // src: bufferG, dst bufferHDR.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboG);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboHDR);
    glBlitFramebuffer(0, 0, SCREEN_W, SCREEN_H, 0, 0, SCREEN_W, SCREEN_H,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHDR);
    {
      shaderLightBox.use();
      shaderLightBox.setMat4fv("projection", glm::value_ptr(projection));
      shaderLightBox.setMat4fv("view", glm::value_ptr(view));
      for (unsigned int i = 0; i < lightPositions.size(); i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPositions[i]);
        model = glm::scale(model, glm::vec3(0.1f));
        shaderLightBox.setMat4fv("model", glm::value_ptr(model));
        shaderLightBox.setVec3fv("lightColor", glm::value_ptr(lightColors[i]));
        renderCube();
      }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Tone mapping.
    shaderHDR.use();
    shaderHDR.setFloat("exposure", exposure);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texHDRColor);
    renderQuad();

    /// @brief Debugging.
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // {
    //   shaderTexture.use();
    //   glActiveTexture(GL_TEXTURE0);
    //   glBindTexture(GL_TEXTURE_2D, texSSAOBlurColor);
    //   renderQuad();
    // }
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
  if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    ssao = 0;
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    ssao = 1;
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

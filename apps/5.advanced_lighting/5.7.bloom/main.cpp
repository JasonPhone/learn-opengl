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
// Bloom switch.
int bloom = 0;
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
  /**
   * @brief init glfw window
   */
  glfwInit();                                    // init GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.x
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL x.3
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE); // use core profile

  // Enable MSAA.
  glfwWindowHint(GLFW_SAMPLES, 4);
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

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  /// @brief Render initializaion
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  // glEnable(GL_FRAMEBUFFER_SRGB);

  // Setup Dear ImGui context
  printf("imgui context\n");
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  printf("binding\n");
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char *glsl_version = "#version 330";
  ImGui_ImplOpenGL3_Init(glsl_version);

  glViewport(0, 0, SCREEN_W, SCREEN_H);
  // Register the resize callback
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  /// @brief build shader program
  // Both output to two color buffers by luminance.
  Shader shaderScene{"../shaders/scene.vert", "../shaders/scene.frag"};
  Shader shaderLight{"../shaders/scene.vert", "../shaders/light.frag"};
  Shader shaderMix{"../shaders/empty.vert", "../shaders/mix.frag"};
  Shader shaderBlur{"../shaders/empty.vert", "../shaders/blur.frag"};
  Shader shaderTex{"../shaders/empty.vert", "../shaders/texture.frag"};

  /// @brief Textures
  GLuint texWood = loadTexture("../textures/wood.png", true);
  GLuint texBox = loadTexture("../textures/container2_diffuse.png", true);
  shaderScene.use();
  shaderScene.setInt("diffuseTexture", 0);
  shaderMix.use();
  shaderMix.setInt("scene", 0);
  shaderMix.setInt("bloomBlur", 1);
  shaderBlur.use();
  shaderBlur.setInt("image", 0);
  shaderTex.use();
  shaderTex.setInt("texture1", 0);

  std::vector<glm::vec3> lightPositions;
  lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
  lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
  lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
  lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
  std::vector<glm::vec3> lightColors;
  lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
  lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
  lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
  lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));

  /// @brief Configure float framebuffer.
  GLuint hdrFBO;
  glGenFramebuffers(1, &hdrFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  // Color buffer.
  GLuint colorBuffers[2];
  constexpr GLenum attachments[2] = {GL_COLOR_ATTACHMENT0,
                                     GL_COLOR_ATTACHMENT1};
  glGenTextures(2, colorBuffers);
  for (unsigned int i = 0; i < 2; i++) {
    glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_W, SCREEN_H, 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Clamp to the edge as the blur filter would
    // otherwise sample repeated texture values
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Attach texture to hdrFBO.
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D,
                           colorBuffers[i], 0);
  }
  glDrawBuffers(2, attachments);
  // Depth buffer (renderbuffer).
  GLuint rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_W,
                        SCREEN_H);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepth);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // Switch buffer for blurring.
  GLuint switchFBOs[2];
  GLuint switchBuffers[2];
  glGenFramebuffers(2, switchFBOs);
  glGenTextures(2, switchBuffers);
  for (unsigned int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, switchFBOs[i]);
    glBindTexture(GL_TEXTURE_2D, switchBuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_W, SCREEN_H, 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           switchBuffers[i], 0);
  }

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    /****** Input ******/
    process_input(window);

    /****** Logic ******/
    glfwPollEvents();
    float curTime = glfwGetTime();
    deltaTime = curTime - lastFrame;
    lastFrame = curTime;

    // lightPos = glm::vec3{lightPos.x, glm::sin(curTime) * 5, lightPos.z};
    lightPos = glm::vec3{
        // glm::sin(curTime) * 5, //
        lightPos.x, //
        lightPos.y, //
        lightPos.z  //
    };

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
      ImGui::Begin("Bloom");
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

      ImGui::Text("Bloom: %c", "01"[bloom]);
      ImGui::Text("Exposure: %.2f", exposure);

      ImGui::End();
    }

    /****** Render ******/
    glClearColor(0, 0, 0, 1);
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    /// @brief Offline rendering into framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
      glm::mat4 projection = glm::perspective(
          glm::radians(camera.fov()), 1.0 * SCREEN_W / SCREEN_H, 0.1, 100.0);
      glm::mat4 view = camera.viewMatrix();
      glm::mat4 model = glm::mat4(1.0f);
      shaderScene.use();
      shaderScene.setMat4fv("projection", glm::value_ptr(projection));
      shaderScene.setMat4fv("view", glm::value_ptr(view));
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texWood);
      // set lighting uniforms
      for (unsigned int i = 0; i < lightPositions.size(); i++) {
        shaderScene.setVec3fv(
            ("lights[" + std::to_string(i) + "].Position").c_str(),
            glm::value_ptr(lightPositions[i]));
        shaderScene.setVec3fv(
            ("lights[" + std::to_string(i) + "].Color").c_str(),
            glm::value_ptr(lightColors[i]));
      }
      shaderScene.setVec3fv("viewPos", glm::value_ptr(camera.cameraPosition()));
      // One large cube as floor.
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
      model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
      shaderScene.setMat4fv("model", glm::value_ptr(model));
      renderCube();
      // Multiple cubes as the scenery.
      glBindTexture(GL_TEXTURE_2D, texBox);
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
      model = glm::scale(model, glm::vec3(0.5f));
      shaderScene.setMat4fv("model", glm::value_ptr(model));
      renderCube();
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
      model = glm::scale(model, glm::vec3(0.5f));
      shaderScene.setMat4fv("model", glm::value_ptr(model));
      renderCube();
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
      model = glm::rotate(model, glm::radians(60.0f),
                          glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
      shaderScene.setMat4fv("model", glm::value_ptr(model));
      renderCube();
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
      model = glm::rotate(model, glm::radians(23.0f),
                          glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
      model = glm::scale(model, glm::vec3(1.25));
      shaderScene.setMat4fv("model", glm::value_ptr(model));
      renderCube();
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
      model = glm::rotate(model, glm::radians(124.0f),
                          glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
      shaderScene.setMat4fv("model", glm::value_ptr(model));
      renderCube();
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
      model = glm::scale(model, glm::vec3(0.5f));
      shaderScene.setMat4fv("model", glm::value_ptr(model));
      renderCube();

      // Bright cubes as light sources.
      shaderLight.use();
      shaderLight.setMat4fv("projection", glm::value_ptr(projection));
      shaderLight.setMat4fv("view", glm::value_ptr(view));
      for (unsigned int i = 0; i < lightPositions.size(); i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lightPositions[i]));
        model = glm::scale(model, glm::vec3(0.1f));
        shaderLight.setMat4fv("model", glm::value_ptr(model));
        shaderLight.setVec3fv("lightColor", glm::value_ptr(lightColors[i]));
        renderCube();
      }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /// @brief Blurring.
    bool horizontal = true, notDumped = true;
    int amount = 10;
    shaderBlur.use();
    for (int i = 0; i < amount; i++) {
      glBindFramebuffer(GL_FRAMEBUFFER, switchFBOs[horizontal]);
      shaderBlur.setInt("horizontal", horizontal);
      glBindTexture(GL_TEXTURE_2D,
                    notDumped ? colorBuffers[1] : switchBuffers[!horizontal]);
      renderQuad();
      horizontal = !horizontal;
      if (notDumped)
        notDumped = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /// @brief Render color buffer to 2D quad and do tone mapping.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderMix.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, switchBuffers[!horizontal]);
    shaderMix.setBool("bloom", bloom);
    shaderMix.setFloat("exposure", exposure);
    renderQuad();
    // shaderTex.use();
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, switchBuffers[!horizontal]);
    // renderQuad();

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
    bloom = 0;
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    bloom = 1;
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
        // positions        // texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
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
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

glm::vec3 camPos = glm::vec3(0.0f, 2.0f, 0.0f);
// Front direction, not looking at point
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
Camera camera{camPos, camFront, camUp};

glm::vec3 lightPos(0.0f, 0.0f, 2.0f);
// Up and down key to adjust.
float heightScale = 0;
// Parallax mapping method;
int parallaxMethod = 1;

float deltaTime = 0;
float lastFrame = 0;

// Data
void framebuffer_size_callback(GLFWwindow *, int w, int h);
void process_input(GLFWwindow *window);
void mouse_move_callback(GLFWwindow *, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow *, double, double yoffset);
void renderScene(const Shader &shader);
void renderCube();
void renderPlane();
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
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_FRAMEBUFFER_SRGB);

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
  // Shader shaderPlane{"../shaders/plane.vert", "../shaders/plane.frag"};
  Shader shaderPlane{"../shaders/shader_plane.vert",
                     "../shaders/shader_plane.geom",
                     "../shaders/shader_plane.frag"};

  /// @brief Textures
  GLuint texDiff = loadTexture("../textures/box_diff.png", true);
  GLuint mapNormal = loadTexture("../textures/box_normal.png", false);
  GLuint mapDisp = loadTexture("../textures/box_disp.png", false);
  shaderPlane.use();
  shaderPlane.setInt("texDiffuse", 0);
  shaderPlane.setInt("mapNormal", 1);
  shaderPlane.setInt("mapDisplace", 2);

  /// @brief Render initializaion
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
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
      ImGui::Begin("Parallax Mapping");

      ImGui::Text("%.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

      ImGui::Text("Light position: (%.2f, %.2f, %.2f)", lightPos.x, lightPos.y,
                  lightPos.z);
      ImGui::Text("Camera position: (%.2f, %.2f, %.2f)",
                  camera.cameraPosition().x, camera.cameraPosition().y,
                  camera.cameraPosition().z);
      ImGui::Text("Camera facing: (%.2f, %.2f, %.2f)", camera.cameraFront().x,
                  camera.cameraFront().y, camera.cameraFront().z);
      ImGui::Text("Height scale: %.5f", heightScale);
      ImGui::Text("Parallax method: %d", parallaxMethod);

      ImGui::End();
    }

    /****** Render ******/
    glClearColor(0.01, 0.01, 0.01, 1.0);
    double zNear = 0.01, zFar = 100;
    /// @brief Common rendering pass.
    // Draw brickwalls.
    glViewport(0, 0, SCREEN_W, SCREEN_H);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 proj = glm::perspective(glm::radians(camera.fov()),
                                      1.0 * SCREEN_W / SCREEN_H, zNear, zFar);
    glm::mat4 view = camera.viewMatrix();
    shaderPlane.use();
    shaderPlane.setMat4fv("view", glm::value_ptr(view));
    shaderPlane.setMat4fv("proj", glm::value_ptr(proj));
    shaderPlane.setVec3fv("lightPos", glm::value_ptr(lightPos));
    shaderPlane.setVec3fv("viewPos", glm::value_ptr(camera.cameraPosition()));
    shaderPlane.setFloat("farPlane", zFar);
    shaderPlane.setFloat("heightScale", heightScale);
    shaderPlane.setInt("parallaxMethod", parallaxMethod);
    shaderPlane.setBool("reverseNormal", false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mapNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mapDisp);
    renderScene(shaderPlane);

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
    heightScale = heightScale + 0.0001 > 0.5 ? 0.5 : heightScale + 0.0001;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    heightScale = heightScale - 0.0001 < 0 ? 0 : heightScale - 0.0001;
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    parallaxMethod = 1;
  if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    parallaxMethod = 2;
  if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    parallaxMethod = 3;
}
void mouse_move_callback(GLFWwindow *, double xpos, double ypos) {
  camera.turn(xpos, ypos);
}

void mouse_scroll_callback(GLFWwindow *, double, double yoffset) {
  camera.zoom(yoffset);
}
void renderScene(const Shader &shader) {
  // Box.
  float angle = glm::radians(90.f);
  // float angle = glm::radians(glfwGetTime());
  glm::mat4 model = glm::mat4{1.0};
  model = glm::translate(model, glm::vec3{0, 0, -5});
  model = glm::rotate(model, angle, glm::vec3{1, 0, 0});
  model = glm::scale(model, glm::vec3{3, 3, 3});
  shader.setMat4fv("model", glm::value_ptr(model));
  renderPlane();
  // model = glm::translate(glm::mat4{1}, glm::vec3{0, 0, -2});
  // model = glm::rotate(model, angle, glm::vec3{1, 0, 0});
  // shader.setMat4fv("model", glm::value_ptr(model));
  // renderPlane();
}
GLuint planeVAO, planeVBO;
void renderPlane() {
  if (planeVAO == 0) {
    // clang-format off
    float planeVertices[] = {
      // positions            // normals         // texcoords
       1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
      -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

       1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
      -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
       1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f
    };
    // clang-format on
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * FLOAT_SIZE,
                          (void *)(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * FLOAT_SIZE,
                          (void *)(3 * FLOAT_SIZE));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * FLOAT_SIZE,
                          (void *)(6 * FLOAT_SIZE));
    glBindVertexArray(0);
  }
  glBindVertexArray(planeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
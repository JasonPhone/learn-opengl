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
#include "learn-opengl/prefab.h"

constexpr int SCREEN_W = 1024;
constexpr int SCREEN_H = 768;

glm::vec3 camPos = glm::vec3(0.0f, 2.0f, 0.0f);
// Front direction, not looking at point
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
Camera camera{camPos, camFront, camUp};

glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

float deltaTime = 0;
float lastFrame = 0;

// Data
std::vector<glm::vec3> boxPos;
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
  glfwInit();                                     // init GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // OpenGL 3.x
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // OpenGL x.3
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE);  // use core profile

  // Enable MSAA.
  glfwWindowHint(GLFW_SAMPLES, 4);
  // Get the glfw window
  GLFWwindow *window =
      glfwCreateWindow(SCREEN_W, SCREEN_H, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {  // check
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
  Shader shaderObject{"../shaders/object.vert", "../shaders/object.frag"};
  // Shader shaderLight{"../shaders/light.vert",
  //                     "../shaders/light.frag"};
  Shader shaderDepthMap{"../shaders/shadowmap.vert",
                        "../shaders/shadowmap.geom",
                        "../shaders/shadowmap.frag"};

  /// @brief Textures
  GLuint texWood = loadTexture("../textures/wood.png", true);
  shaderObject.use();
  shaderObject.setInt("texDiffuse", 0);
  shaderObject.setInt("texShadow", 1);
  for (int i = 0; i < 20; i++) {
    glm::vec3 pos{0};
    pos.x = (1.0 * rand() / RAND_MAX - 0.5) * 5;
    pos.y = (1.0 * rand() / RAND_MAX - 0.5) * 5;
    pos.z = (1.0 * rand() / RAND_MAX - 0.5) * 5;
    pos.x += pos.x < 0 ? -2 : 2;
    pos.y += pos.y < 0 ? -2 : 2;
    pos.z += pos.z < 0 ? -2 : 2;
    boxPos.push_back(pos);
  }

  /// @brief Shadow map.
  constexpr int SHADOW_W = 2048, SHADOW_H = 2048;
  GLuint depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  GLuint depthCubeMap;
  // Generate texture.
  glGenTextures(1, &depthCubeMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
  // Attach 6 textures, one per direction.
  for (int i = 0; i < 6; i++)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                 SHADOW_W, SHADOW_H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  // Bind texture to frame buffer.
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  /// @brief Render initializaion
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  // Render loop
  while (!glfwWindowShouldClose(window)) {
    /****** Input ******/
    process_input(window);

    /****** Logic ******/
    glfwPollEvents();
    float cur_frame = glfwGetTime();
    deltaTime = cur_frame - lastFrame;
    lastFrame = cur_frame;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
      ImGui::Begin("Point Shadow");

      ImGui::Text("%.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

      ImGui::Text("Camera position: (%.2f, %.2f, %.2f)",
                  camera.cameraPosition().x, camera.cameraPosition().y,
                  camera.cameraPosition().z);
      ImGui::Text("Camera front: (%.2f, %.2f, %.2f)", camera.cameraFront().x,
                  camera.cameraFront().y, camera.cameraFront().z);

      ImGui::End();
    }

    /****** Render ******/
    glClearColor(0.01, 0.01, 0.01, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    double zNear = 0.01, zFar = 100, aspect = 1.0 * SHADOW_W / SHADOW_H;
    // Point light.
    glm::mat4 proj = glm::perspective(glm::radians(90.0), aspect, zNear, zFar);
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(
        proj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(
        proj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(
        proj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f),
                           glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(
        proj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f),
                           glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(
        proj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f),
                           glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(
        proj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f),
                           glm::vec3(0.0f, -1.0f, 0.0f)));

    /// @brief Shadowmapping pass.
    glViewport(0, 0, SHADOW_W, SHADOW_H);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    {
      glClear(GL_DEPTH_BUFFER_BIT);
      shaderDepthMap.use();
      for (unsigned int i = 0; i < 6; ++i) {
        std::string name = "shadowMats[" + std::to_string(i) + "]";
        shaderDepthMap.setMat4fv(name.c_str(), &(shadowTransforms[i][0][0]));
      }
      // shaderDepthMap.setMat4fv("shadowMats",
      //                          glm::value_ptr(shadowTransforms[0]));
      shaderDepthMap.setVec3fv("lightPos", glm::value_ptr(lightPos));
      shaderDepthMap.setFloat("farPlane", zFar);
      renderScene(shaderDepthMap);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /// @brief Common rendering pass.
    glViewport(0, 0, SCREEN_W, SCREEN_H);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    proj = glm::perspective(glm::radians(camera.fov()),
                            1.0 * SCREEN_W / SCREEN_H, zNear, zFar);
    glm::mat4 view = camera.viewMatrix();
    shaderObject.use();
    shaderObject.setMat4fv("view", glm::value_ptr(view));
    shaderObject.setMat4fv("proj", glm::value_ptr(proj));
    shaderObject.setVec3fv("lightPos", glm::value_ptr(lightPos));
    shaderObject.setVec3fv("viewPos", glm::value_ptr(camera.cameraPosition()));
    shaderObject.setFloat("farPlane", zFar);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texWood);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
    renderScene(shaderObject);

    if (glCheckError() != GL_NO_ERROR) break;
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
}
void mouse_move_callback(GLFWwindow *, double xpos, double ypos) {
  camera.turn(xpos, ypos);
}

void mouse_scroll_callback(GLFWwindow *, double, double yoffset) {
  camera.zoom(yoffset);
}
void renderScene(const Shader &shader) {
  // Box.
  glm::mat4 model = glm::scale(glm::mat4{1.0}, glm::vec3{10, 10, 10});
  shader.setMat4fv("model", glm::value_ptr(model));
  shader.setBool("reverseNormal", true);
  renderCube();
  // Cubes
  shader.setBool("reverseNormal", false);
  for (size_t i = 0; i < boxPos.size(); i++) {
    model = glm::translate(glm::mat4{1}, boxPos[i]);
    glm::vec3 v = (i == 0) ? glm::vec3{1, 0, 0}
                           : (i == 1 ? glm::vec3{0, 1, 0} : glm::vec3{0, 0, 1});
    model = glm::rotate(model, glm::radians(i * 10.0f + 10), v);
    model = glm::scale(model, {0.5, 0.5, 0.5});
    shader.setMat4fv("model", glm::value_ptr(model));
    renderCube();
  }
}
GLuint cubeVAO = 0;
GLuint cubeVBO = 0;
void renderCube() {
  if (cubeVAO == 0) {
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(prefab::cube), prefab::cube,
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
  glBindVertexArray(cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}
GLuint planeVAO, planeVBO;
void renderPlane() {
  if (planeVAO == 0) {
    // clang-format off
    float planeVertices[] = {
      // positions            // normals         // texcoords
       10.0f,  0.0f,  10.0f,  0.0f, 10.0f, 0.0f, 10.0f,  0.0f,
      -10.0f,  0.0f,  10.0f,  0.0f, 10.0f, 0.0f,  0.0f,  0.0f,
      -10.0f,  0.0f, -10.0f,  0.0f, 10.0f, 0.0f,  0.0f, 10.0f,

       10.0f,  0.0f,  10.0f,  0.0f, 10.0f, 0.0f, 10.0f,  0.0f,
      -10.0f,  0.0f, -10.0f,  0.0f, 10.0f, 0.0f,  0.0f, 10.0f,
       10.0f,  0.0f, -10.0f,  0.0f, 10.0f, 0.0f, 10.0f, 10.0f
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
GLuint quadVAO = 0;
GLuint quadVBO;
void renderQuad() {
  if (quadVAO == 0) {
    // clang-format off
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
    // clang-format on
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
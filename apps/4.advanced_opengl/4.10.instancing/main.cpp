#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <vector>

#include "ImGui/backend/imgui_impl_glfw.h"
#include "ImGui/backend/imgui_impl_opengl3.h"
#include "ImGui/imgui.h"
#include "learn-opengl/camera.h"
#include "learn-opengl/gl_utility.h"
#include "learn-opengl/model.h"
#include "learn-opengl/shader.h"

// Settings
constexpr unsigned int SCR_WIDTH = 900;
constexpr unsigned int SCR_HEIGHT = 600;
// Timing
float delta_time = 0.0f;
float last_frame = 0.0f;

// Camera
Camera camera(glm::vec3(20.0f, 20.0f, 20.0f));

// Vertices
constexpr int INSTANCE_NUM = 40000;
// clang-format off
float points[] = {
  // position      // color
  -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
   0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
  -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

  -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
   0.05f, -0.05f,  0.0f, 1.0f, 0.0f,   
   0.05f,  0.05f,  0.0f, 1.0f, 1.0f
};
// clang-format on
void framebuffer_size_callback(GLFWwindow *, int w, int h) {
  glViewport(0, 0, w, h);
}
void mouse_move_callback(GLFWwindow *, double xpos, double ypos) {
  camera.turn(xpos * 1.5, ypos * 1.5);
}
void mouse_scroll_callback(GLFWwindow *, double, double yoffset) {
  camera.zoom(yoffset);
}
void process_input(GLFWwindow *window) {
  // Close window on Esc pressed
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  // Camera move
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::FORWARD, delta_time);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::BACKWARD, delta_time);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::LEFT, delta_time);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::RIGHT, delta_time);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::UP, delta_time);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera.move(MOVE_DIRECTION::DOWN, delta_time);
  // Camera turn.
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) camera.turn_delta(0, -2);
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) camera.turn_delta(0, 2);
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) camera.turn_delta(-2, 0);
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) camera.turn_delta(2, 0);
}
void normalDraw(GLFWwindow *window);
void instancedDraw(GLFWwindow *window);

int main() {
  // Initialize and configure glfw
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  camera.set_move_speed(3);

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
  // glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwSetScrollCallback(window, mouse_scroll_callback);

  // Capture mouse
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

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

  // Configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  normalDraw(window);
  // instancedDraw(window);

  printf("clean\n");
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

void normalDraw(GLFWwindow *window) {
  // Build and compile shaders
  // -------------------------
  Shader shader("../shader/quad.vert", "../shader/quad.frag");
  // Model.
  Model planet = Model{"../models/planet/planet.obj"};
  Model rock = Model{"../models/rock/rock.obj"};

  glm::mat4 *modelMats = new glm::mat4[INSTANCE_NUM];
  srand(glfwGetTime());
  float radius = 50;
  float offset = 2.5;
  for (int i = 0; i < INSTANCE_NUM; i++) {
    glm::mat4 model = glm::mat4(1.0f);
    // 1. Translate: displace along circle field [-offset, offset].
    float angle = (float)i / (float)INSTANCE_NUM * 360.0f;
    float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    float x = sin(angle) * radius + displacement;
    displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    // Keep height of field smaller compared to width of x and z.
    float y = displacement * 0.4f;
    displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    float z = cos(angle) * radius + displacement;
    model = glm::translate(model, glm::vec3(x, y, z));
    // 2. Scale: scale between 0.05 and 0.25f.
    float scale = (rand() % 20) / 100.0f + 0.05;
    model = glm::scale(model, glm::vec3(scale));
    // 3. Rotation: add random rotation around an axis.
    float rotAngle = (rand() % 360);
    model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

    modelMats[i] = model;
  }

  // Render loop
  // -----------
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while (!glfwWindowShouldClose(window)) {
    // Per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    delta_time = currentFrame - last_frame;
    last_frame = currentFrame;
    // Input
    // -----
    process_input(window);
    glfwPollEvents();
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
      ImGui::Begin("Normal");

      ImGui::Text("%.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

      ImGui::End();
    }

    glClearColor(0.3f, 0.3f, 0.3f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Render
    // ------

    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
    glm::mat4 view = camera.view_matrix();
    shader.use();
    shader.set_mat4fv("projection", glm::value_ptr(projection));
    if (glCheckError() != GL_NO_ERROR) break;
    shader.set_mat4fv("view", glm::value_ptr(view));

    // draw planet
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
    shader.set_mat4fv("model", glm::value_ptr(model));
    planet.draw(shader);

    // draw meteorites
    for (unsigned int i = 0; i < INSTANCE_NUM; i++) {
      shader.set_mat4fv("model", glm::value_ptr(modelMats[i]));
      rock.draw(shader);
    }

    if (glCheckError() != GL_NO_ERROR) break;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }
}
void instancedDraw(GLFWwindow *window) {
  // Build and compile shaders
  // -------------------------
  Shader shaderInstanced("../shader/quad_instance.vert", "../shader/quad.frag");
  Shader shader("../shader/quad.vert", "../shader/quad.frag");
  // Model.
  Model planet = Model{"../models/planet/planet.obj"};
  Model rock = Model{"../models/rock/rock.obj"};

  glm::mat4 *modelMats = new glm::mat4[INSTANCE_NUM];
  srand(glfwGetTime());
  float radius = 50;
  float offset = 2.5;
  for (int i = 0; i < INSTANCE_NUM; i++) {
    glm::mat4 model = glm::mat4(1.0f);
    // 1. Translate: displace along circle field [-offset, offset].
    float angle = (float)i / (float)INSTANCE_NUM * 360.0f;
    float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    float x = sin(angle) * radius + displacement;
    displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    // Keep height of field smaller compared to width of x and z.
    float y = displacement * 0.4f;
    displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
    float z = cos(angle) * radius + displacement;
    model = glm::translate(model, glm::vec3(x, y, z));
    // 2. Scale: scale between 0.05 and 0.25f.
    float scale = (rand() % 20) / 100.0f + 0.05;
    model = glm::scale(model, glm::vec3(scale));
    // 3. Rotation: add random rotation around an axis.
    float rotAngle = (rand() % 360);
    model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

    modelMats[i] = model;
  }
  // vertex buffer object
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, INSTANCE_NUM * sizeof(glm::mat4), &modelMats[0],
               GL_STATIC_DRAW);

  for (unsigned int i = 0; i < rock.mMeshes.size(); i++) {
    GLuint VAO = rock.mMeshes[i].mVAO;
    glBindVertexArray(VAO);
    // vertex attributes
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                          (void *)(1 * vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                          (void *)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                          (void *)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
  }
  // Render loop
  // -----------
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while (!glfwWindowShouldClose(window)) {
    // Per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    delta_time = currentFrame - last_frame;
    last_frame = currentFrame;
    // Input
    // -----
    process_input(window);
    glfwPollEvents();
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
      ImGui::Begin("Instanced");

      ImGui::Text("%.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

      ImGui::End();
    }

    glClearColor(0.3f, 0.3f, 0.3f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Render
    // ------

    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
    glm::mat4 view = camera.view_matrix();
    shader.use();
    shader.set_mat4fv("projection", glm::value_ptr(projection));
    shader.set_mat4fv("view", glm::value_ptr(view));

    // draw planet
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
    shader.set_mat4fv("model", glm::value_ptr(model));
    planet.draw(shader);

    // draw meteorites
    shaderInstanced.use();
    shaderInstanced.set_mat4fv("projection", glm::value_ptr(projection));
    shaderInstanced.set_mat4fv("view", glm::value_ptr(view));
    if (glCheckError() != GL_NO_ERROR) break;
    for (unsigned int i = 0; i < rock.mMeshes.size(); i++) {
      rock.drawInstanced(shaderInstanced, INSTANCE_NUM);
      // glDrawElementsInstanced(GL_TRIANGLES, rock.mMeshes[i].mIndices.size(),
      //                         GL_UNSIGNED_INT, 0, INSTANCE_NUM);
    }

    if (glCheckError() != GL_NO_ERROR) break;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }
}
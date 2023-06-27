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
#include "learn-opengl/camera.h"
#include "learn-opengl/gl_utility.h"
#include "learn-opengl/shader.h"

constexpr int SCR_W = 800;
constexpr int SCR_H = 600;

glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, 5.0f);
// Front direction, not looking at point
glm::vec3 cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
Camera camera{cam_pos, cam_front, cam_up};

glm::vec3 light_pos(1.5f, 1.5f, 4.0f);
glm::vec3 light_color(1.0f, 1.0f, 1.0f);
glm::vec3 obj_color(1.0f, 0.5f, 0.31f);

float delta_time = 0;
float last_frame = 0;

// vertex data
// clang-format off
  float cube[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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
void mouse_move_callback(GLFWwindow *window, double xpos, double ypos) {
  camera.turn(xpos, ypos);
}

void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.zoom(yoffset);
}

int main() {
  /**
   * @brief init glfw window
   */
  glfwInit();                                     // init GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // OpenGL 3.x
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // OpenGL x.3
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE);  // use core profile
  // Get the glfw window
  GLFWwindow *window =
      glfwCreateWindow(SCR_W, SCR_H, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {  // check
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  // Mouse input
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetScrollCallback(window, mouse_scroll_callback);

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

  glViewport(0, 0, 800, 600);
  // Register the resize callback
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  /**
   * @brief build shader program
   */
  Shader shader_light{"../shaders/shader_light.vs",
                      "../shaders/shader_light.fs"};
  Shader shader_cube{"../shaders/shader_cube.vs", "../shaders/shader_cube.fs"};

  /**
   * @brief VAO and VBO
   */
  // prepare objects
  GLuint VAO_cube, VBO_cube;
  glGenBuffers(1, &VBO_cube);
  glGenVertexArrays(1, &VAO_cube);
  // start to record
  glBindVertexArray(VAO_cube);

  // buffer VBO
  // Cube
  glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
  // vertex attrib: position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // vertex attrib: vertex normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  // Light
  GLuint VAO_light;
  glGenVertexArrays(1, &VAO_light);
  glBindVertexArray(VAO_light);
  // Reuse vertex data of cube
  glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  /**
   * @brief Render initializaion
   */
  glEnable(GL_DEPTH_TEST);
  // Render loop
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0., 0., 0., 1.0);  // rgba, used to clear viewport
    glClear(GL_COLOR_BUFFER_BIT |   // Clear color buffer
            GL_DEPTH_BUFFER_BIT);   // Clear depth buffer

    /****** Input ******/
    process_input(window);

    /****** Logic ******/
    glfwPollEvents();
    float cur_frame = glfwGetTime();
    delta_time = cur_frame - last_frame;
    last_frame = cur_frame;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
      ImGui::Begin("Anti Aliasing");

      ImGui::Text("%.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

      ImGui::End();
    }

    float time = glfwGetTime();
    float offset = sin(time) * 2;
    glm::vec3 light_dypos(light_pos.x, light_pos.y, offset);

    /****** Render ******/
    // Cube
    glBindVertexArray(VAO_cube);
    glm::mat4 view = camera.view_matrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.fov()),
                                      1.0 * SCR_W / SCR_H, 0.1, 100.0);
    glm::mat4 model{1};
    model = glm::rotate(model, glm::radians(time) * 10, glm::vec3(0, 1, 0));
    glm::mat4 normal_mat = glm::inverse(glm::transpose(model));
    shader_cube.use();
    shader_cube.set_mat4fv("view", glm::value_ptr(view));
    shader_cube.set_mat4fv("proj", glm::value_ptr(proj));
    shader_cube.set_mat4fv("model", glm::value_ptr(model));
    shader_cube.set_mat4fv("normal_mat", glm::value_ptr(normal_mat));
    shader_cube.set_vec3fv("view_pos", glm::value_ptr(camera.camera_position()));

    shader_cube.set_vec3f("mat.ambient", 1.0f, 0.5f, 0.31f);
    shader_cube.set_vec3f("mat.diffuse", 1.0f, 0.5f, 0.31f);
    shader_cube.set_vec3f("mat.specular", 0.5f, 0.5f, 0.5f);
    shader_cube.set_float("mat.shininess", 32.0f);

    shader_cube.set_vec3fv("lit.position", glm::value_ptr(light_dypos));
    shader_cube.set_vec3f("lit.ambient", 0.2f, 0.2f, 0.2f);
    shader_cube.set_vec3f("lit.diffuse", 0.5f, 0.5f, 0.5f);
    shader_cube.set_vec3f("lit.specular", 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Light
    glBindVertexArray(VAO_light);
    shader_light.use();
    view = camera.view_matrix();
    model = glm::mat4{1};
    model = glm::translate(model, light_dypos);
    model = glm::scale(model, glm::vec3(0.2f));
    shader_light.set_mat4fv("view", glm::value_ptr(view));
    shader_light.set_mat4fv("proj", glm::value_ptr(proj));
    shader_light.set_mat4fv("model", glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    if (glCheckError() != GL_NO_ERROR) break;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);  // We use double buffer
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

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "learn-opengl/gl_error_checker.h"
#include "learn-opengl/shader.h"
#include "learn-opengl/texture.h"
#include "learn-opengl/camera.h"
#include "learn-opengl/prefab.h"
#include "stb_image.h"

constexpr int SCR_W = 800;
constexpr int SCR_H = 600;

glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, 3.0f);
// Front direction, not looking at point
glm::vec3 cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
Camera cam{cam_pos, cam_front, cam_up};

float delta_time = 0;
float last_frame = 0;

void framebuffer_size_callback(GLFWwindow *window, int w, int h) {
  glViewport(0, 0, w, h);
}
void process_input(GLFWwindow *window) {
  // Close window on Esc pressed
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  // Camera move
  float cam_speed = cam.move_speed() * delta_time;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::FORWARD, delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::BACKWARD, delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::LEFT, delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::RIGHT, delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::UP, delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::DOWN, delta_time);
  }
}
void mouse_move_callback(GLFWwindow *window, double xpos, double ypos) {
  cam.turn(xpos, ypos);
}

void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  cam.zoom(yoffset);
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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwSetScrollCallback(window, mouse_scroll_callback);

  // use GLAD to manage OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {  // check
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  glViewport(0, 0, 800, 600);
  // Register the resize callback
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  /**
   * @brief build shader program
   */
  Shader shader{"./shader.vs", "./shader.fs"};

  /**
   * @brief Setup texture
   */
  stbi_set_flip_vertically_on_load(true);
  // generate texture0
  Texture2D tex0("./opengl_logo.png");
  tex0.setParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  tex0.setParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
  tex0.setParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  tex0.setParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  Texture2D tex1("./awesomeface.png");
  tex1.setParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
  tex1.setParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
  tex1.setParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  tex1.setParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  /**
   * @brief VAO and VBO
   * ------------------
   */
  // vertex data
  // clang-format off
  glm::vec3 cube_pos[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
  };
  // clang-format on
  // prepare objects
  GLuint VAO, VBO;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  // start to record
  glBindVertexArray(VAO);

  // buffer VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(prefab::cube), prefab::cube, GL_STATIC_DRAW);
  // vertex attrib: position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // vertex attrib: tex coord
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // finish the recording
  glBindVertexArray(0);

  /**
   * @brief Render initializaion
   */
  // render loop
  shader.use();
  // tell the shader which texture unit they are
  shader.set_int("texture0", 0);
  shader.set_int("texture1", 1);

  glEnable(GL_DEPTH_TEST);
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.3, 0.3, 0.3, 1.0);  // rgba, used to clear viewport
    glClear(GL_COLOR_BUFFER_BIT |      // Clear color buffer
            GL_DEPTH_BUFFER_BIT);      // Clear depth buffer
    // binding
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0.ID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1.ID);
    glBindVertexArray(VAO);

    /****** Input ******/
    process_input(window);

    /****** Instruction ******/

    /****** Logic ******/
    glfwPollEvents();
    float cur_frame = glfwGetTime();
    delta_time = cur_frame - last_frame;
    last_frame = cur_frame;
    // std::cout << "delta time: " << delta_time << "\n";

    /****** Render ******/
    glm::mat4 view{1};
    double time = glfwGetTime();
    double radius = 10;
    double cam_x = sin(time) * radius;
    double cam_z = cos(time) * radius;
    view = cam.view_matrix();

    glm::mat4 proj{1};
    GLuint loc;
    proj = glm::perspective(glm::radians(cam.fov()), 1.0 * SCR_W / SCR_H, 0.1,
                            100.0);
    loc = glGetUniformLocation(shader.ID, "view");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

    loc = glGetUniformLocation(shader.ID, "proj");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(proj));

    for (int i = 0; i < 10; i++) {
      glm::mat4 model{1};
      model = glm::translate(model, cube_pos[i]);
      model = glm::rotate(model, float(20.0 * i), glm::vec3{1, 0.3, 0.5});
      loc = glGetUniformLocation(shader.ID, "model");
      glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    if (glCheckError() != GL_NO_ERROR) break;
    glfwSwapBuffers(window);  // We use double buffer
  }
  glfwTerminate();
  std::cout << "done\n";
  return 0;
}

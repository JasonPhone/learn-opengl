#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "learn-opengl/logger.h"
#include "learn-opengl/shader.h"
#include "learn-opengl/camera.h"
#include "learn-opengl/texture.h"
#include "stb_image.h"

constexpr int SCR_W = 800;
constexpr int SCR_H = 600;

glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, 5.0f);
// Front direction, not looking at point
glm::vec3 cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
Camera cam{cam_pos, cam_front, cam_up};

glm::vec3 light_pos(1.5f, 1.5f, 4.0f);
glm::vec3 light_color(1.0f, 1.0f, 1.0f);
glm::vec3 obj_color(1.0f, 0.5f, 0.31f);

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
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cam.move(MOVE_DIRECTION::FORWARD, delta_time);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cam.move(MOVE_DIRECTION::BACKWARD, delta_time);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cam.move(MOVE_DIRECTION::LEFT, delta_time);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cam.move(MOVE_DIRECTION::RIGHT, delta_time);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    cam.move(MOVE_DIRECTION::UP, delta_time);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    cam.move(MOVE_DIRECTION::DOWN, delta_time);
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
  Shader shader_light{"./shader_light.vs", "./shader_light.fs"};
  Shader shader_cube{"./shader_cube.vs", "./shader_cube.fs"};

  /**
   * @brief VAO and VBO
   */
  // vertex data
  // clang-format off
  float cube[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
  };
  glm::vec3 cube_pos[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -5.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -2.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
  };
  // clang-format on
  // prepare objects
  GLuint VAO_cube, VBO;
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO_cube);

  // Reuse vertex data of cube
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
  // buffer VBO
  // Cube
  glBindVertexArray(VAO_cube);
  // vertex attrib: position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // vertex attrib: vertex normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // vertex attrib: texture coord
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glBindVertexArray(0);

  // Light
  GLuint VAO_light;
  glGenVertexArrays(1, &VAO_light);
  glBindVertexArray(VAO_light);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  /**
   * @brief Material map
   */
  // Diffuse map
  // Texture2D diffuse_map("./container2_diffuse.png");
  // Texture2D specular_map("./container2_specular.png");
  GLuint diffuse_map = load_texture("./container2_diffuse.png");
  GLuint specular_map = load_texture("./container2_specular.png");
  GLuint emission_map = load_texture("./matrix.jpg");

  /**
   * @brief Render initializaion
   */
  glEnable(GL_DEPTH_TEST);
  // Set sampler2D
  shader_cube.use();
  shader_cube.set_int("mat.diffuse", 0);
  shader_cube.set_int("mat.specular", 1);
  shader_cube.set_int("mat.emission", 2);
  // Render loop
  while (!glfwWindowShouldClose(window)) {
    /****** Logic ******/
    glfwPollEvents();
    float cur_frame = glfwGetTime();
    delta_time = cur_frame - last_frame;
    last_frame = cur_frame;

    float time = glfwGetTime();
    float offset = sin(time) * 2;
    glm::vec3 light_dypos(light_pos.x, offset, light_pos.z);

    /****** Input ******/
    process_input(window);

    /****** Instruction ******/

    /****** Render ******/
    glClearColor(0.1, 0.1, 0.1, 1.0);  // rgba, used to clear viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm:: mat4 view{1}, proj{1}, model{1};
    // Cube
    for (int i = 0; i < 10; i++) {
      model = glm::mat4{1};
      view = cam.view_matrix();
      proj = glm::perspective(glm::radians(cam.fov()),
                                        1.0 * SCR_W / SCR_H, 0.1, 100.0);
      model = glm::translate(model, cube_pos[i]);
      model = glm::rotate(model, glm::radians(time) * (10 + i), glm::vec3(0, 1, 0));
      glm::mat4 normal_mat = glm::inverse(glm::transpose(model));
      shader_cube.use();
      shader_cube.set_mat4fv("view", glm::value_ptr(view));
      shader_cube.set_mat4fv("proj", glm::value_ptr(proj));
      shader_cube.set_mat4fv("model", glm::value_ptr(model));
      shader_cube.set_mat4fv("normal_mat", glm::value_ptr(normal_mat));
      shader_cube.set_vec3fv("view_pos", glm::value_ptr(cam.camera_position()));

      shader_cube.set_float("mat.shininess", 64.0f);

      shader_cube.set_vec3fv("lit.position", glm::value_ptr(cam.camera_position() + glm::vec3(0, -0.5, 0)));
      shader_cube.set_vec3fv("lit.direction", glm::value_ptr(cam.camera_front()));
      shader_cube.set_vec3f("lit.ambient", 0.2f, 0.2f, 0.2f);
      shader_cube.set_vec3f("lit.diffuse", 0.5f, 0.5f, 0.5f);
      shader_cube.set_vec3f("lit.specular", 1.0f, 1.0f, 1.0f);

      shader_cube.set_float("lit.constant", 1.0f);
      shader_cube.set_float("lit.linear", 0.09f);
      shader_cube.set_float("lit.quadratic", 0.032f);

      shader_cube.set_float("lit.cutoff_inner", glm::cos(glm::radians(12.5f)));
      shader_cube.set_float("lit.cutoff_outer", glm::cos(glm::radians(17.5f)));

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, diffuse_map);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, specular_map);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, emission_map);

      glBindVertexArray(VAO_cube);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Light
    shader_light.use();
    view = cam.view_matrix();
    model = glm::mat4{1};
    model = glm::translate(model, light_dypos);
    model = glm::scale(model, glm::vec3(0.2f));
    shader_light.set_mat4fv("view", glm::value_ptr(view));
    shader_light.set_mat4fv("proj", glm::value_ptr(proj));
    shader_light.set_mat4fv("model", glm::value_ptr(model));

    glBindVertexArray(VAO_light);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    if (glCheckError() != GL_NO_ERROR) break;
    glfwSwapBuffers(window);  // We use double buffer
  }
  glfwTerminate();
  std::cout << "done\n";
  return 0;
}

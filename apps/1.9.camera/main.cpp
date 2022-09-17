#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "learn-opengl/gl_error_checker.h"
#include "learn-opengl/shader.h"
#include "learn-opengl/camera.h"
#include "stb_image.h"

constexpr int SCR_W = 800;
constexpr int SCR_H = 600;

glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, 3.0f);
// Front direction, not looking at point
glm::vec3 cam_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cam_up = glm::vec3(0.0f, 1.0f, 0.0f);

/**
 * @brief Delta time is the time cost to render last frame.
 * If it's big, we need to go faster in this frame, so
 * delta_time is suitable for the time duration of current frame.
 */
float delta_time = 0;
float last_frame = 0;
/**
 * @brief Last position of mouse
 */
double mouse_last_xpos = SCR_W / 2;
double mouse_last_ypos = SCR_H / 2;
bool mouse_first_capture = true;
double camera_pitch = 0, camera_yaw = 0;
float fov = 45;

void framebuffer_size_callback(GLFWwindow *window, int w, int h) {
  glViewport(0, 0, w, h);
}
void process_input(GLFWwindow *window) {
  // Close window on Esc pressed
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  float cam_speed = 2.5 * delta_time;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cam_pos += cam_speed * cam_front;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cam_pos -= cam_speed * cam_front;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cam_pos += cam_speed * glm::normalize(glm::cross(cam_up, cam_front));
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cam_pos -= cam_speed * glm::normalize(glm::cross(cam_up, cam_front));
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    cam_pos += cam_speed * cam_up;
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    cam_pos -= cam_speed * cam_up;
}
void mouse_move_callback(GLFWwindow *window, double xpos, double ypos) {
  if (mouse_first_capture) {
    mouse_first_capture = false;
    mouse_last_xpos = xpos;
    mouse_last_ypos = ypos;
  }
  double delta_xpos = xpos - mouse_last_xpos;
  double delta_ypos = ypos - mouse_last_ypos;
  delta_ypos = -delta_ypos;

  mouse_last_xpos = xpos;
  mouse_last_ypos = ypos;

  float sensitivity = 0.05;
  camera_pitch += sensitivity * delta_ypos;
  camera_yaw += sensitivity * delta_xpos;

  // Restrict the pitch range
  camera_pitch = camera_pitch <= 89.0 ? camera_pitch : 89.0;
  camera_pitch = camera_pitch >= -89.0 ? camera_pitch : -89.0;

  glm::vec3 front;
  front.x = cos(glm::radians(camera_pitch)) * sin(glm::radians(camera_yaw));
  front.y = sin(glm::radians(camera_pitch));
  front.z = -cos(glm::radians(camera_pitch)) * cos(glm::radians(camera_yaw));

  cam_front = front;
}

void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  fov-= yoffset;
  fov = fov <= 45 ? fov : 45;
  fov = fov >= 1 ? fov : 1;
}

int main() {
  /**
   * @brief init glfw window
   * -----------------------
   */
  glfwInit();  // init GLFW
  // set the OpenGL version
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
  // position of bot-left, and size of viewport
  // It's independent from window size of GLFW
  glViewport(0, 0, 800, 600);
  // Register the resize callback
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  /**
   * @brief build shader program
   * ---------------------------
   */
  Shader shader{"./shader.vs", "./shader.fs"};

  /**
   * @brief setup texture
   * --------------------
   */
  // TODO use a class to do this
  // generate texture0
  GLuint texture0;
  glGenTextures(1, &texture0);
  // bind texture
  glBindTexture(GL_TEXTURE_2D, texture0);
  // set warp and filter method
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image
  stbi_set_flip_vertically_on_load(true);
  int image_w, image_h, image_channel;
  unsigned char *image =
      stbi_load("./opengl_logo.png", &image_w, &image_h, &image_channel, 0);
  if (image) {
    // transfer texture data
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, image_w, image_h, 0, GL_RGBA,
        GL_UNSIGNED_BYTE,
        image);  // now the texture has only base-level(0) image attached
    // generate mipmap for current bind texture
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
  } else {
    std::cerr << "::main: fail to load image\n";
  }
  // texture1 the same
  GLuint texture1;
  glGenTextures(1, &texture1);
  // bind texture
  glBindTexture(GL_TEXTURE_2D, texture1);
  // set warp and filter method
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  image = stbi_load("./awesomeface.png", &image_w, &image_h, &image_channel, 0);
  if (image) {
    // transfer texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_w, image_h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image);
    // generate mipmap for current bind texture
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
  } else {
    std::cerr << "::main: fail to load image\n";
  }

  /**
   * @brief VAO and VBO
   * ------------------
   */
  // vertex data
  // clang-format off
  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,

     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
  };
  glm::vec3 cube_pos[] = {
    // glm::vec3( 0.0f,  0.0f,  0.0f),
    // glm::vec3( 2.0f,  0.0f,  0.0f),
    // glm::vec3( 4.0f,  0.0f,  0.0f),
    // glm::vec3( 6.0f,  0.0f,  0.0f),
    // glm::vec3( 8.0f,  0.0f,  0.0f),
    // glm::vec3(10.0f,  0.0f,  0.0f),
    // glm::vec3(12.0f,  0.0f,  0.0f),
    // glm::vec3(14.0f,  0.0f,  0.0f),
    // glm::vec3(16.0f,  0.0f,  0.0f),
    // glm::vec3(18.0f,  0.0f,  0.0f)

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
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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
    /**
     * Use color 0.3, 0.3, 0.3 to clear color buffer
     * This is to clear thing in last render loop
     * If called outside the render loop, screen will jitter
     */
    glClearColor(0.3, 0.3, 0.3, 1.0);  // rgba, the value used to clear
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT);  // clear the color buffer
    // binding
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
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
    view = glm::lookAt(cam_pos, cam_pos + cam_front, cam_up);

    glm::mat4 proj{1};
    GLuint loc;
    proj = glm::perspective(glm::radians(fov), float(1.0 * SCR_W / SCR_H), 0.1f,
                            100.0f);
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

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "learn-opengl/Camera.h"
#include "learn-opengl/Shader.h"
#include "learn-opengl/gl_utility.h"
#include "learn-opengl/image.h"


constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 600;

glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
// Front direction, not looking at point
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
Camera cam{camPos, camFront, camUp};

/**
 * @brief Delta time is the time cost to render last frame.
 * If it's big, we need to go faster in this frame, so
 * delta_time is suitable for the time duration of current frame.
 */
float deltaTime = 0;
float last_frame = 0;

void framebuffer_size_callback(GLFWwindow *, int w, int h) {
  glViewport(0, 0, w, h);
}
void process_input(GLFWwindow *window) {
  // Close window on Esc pressed
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  // Camera move
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::RIGHT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::UP, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    cam.move(MOVE_DIRECTION::DOWN, deltaTime);
  }
}
void mouse_move_callback(GLFWwindow *, double xpos, double ypos) {
  cam.turn(xpos, ypos);
}

void mouse_scroll_callback(GLFWwindow *, double , double yoffset) {
  cam.zoom(yoffset);
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
  flipVerticalOnLoad(true);
  int image_w, image_h, image_channel;
  unsigned char *image =
      loadImage("./opengl_logo.png", &image_w, &image_h, &image_channel, 0);
  if (image) {
    // transfer texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_w, image_h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image);
    // now the texture has only base-level(0) image attached
    // generate mipmap for current bind texture
    glGenerateMipmap(GL_TEXTURE_2D);
    freeImage(image);
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
  image = loadImage("./awesomeface.png", &image_w, &image_h, &image_channel, 0);
  if (image) {
    // transfer texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_w, image_h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, image);
    // generate mipmap for current bind texture
    glGenerateMipmap(GL_TEXTURE_2D);
    freeImage(image);
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
  shader.setInt("texture0", 0);
  shader.setInt("texture1", 1);

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
    deltaTime = cur_frame - last_frame;
    last_frame = cur_frame;
    // std::cout << "delta time: " << delta_time << "\n";

    /****** Render ******/
    glm::mat4 view{1};
    // double time = glfwGetTime();
    // double radius = 10;
    view = cam.viewMatrix();

    glm::mat4 proj{1};
    GLuint loc;
    proj = glm::perspective(glm::radians(cam.fov()), 1.0 * SCREEN_W / SCREEN_H,
                            0.1, 100.0);
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

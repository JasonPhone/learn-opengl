#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "learn-opengl/gl_error_checker.h"
#include "learn-opengl/shader.h"
#include "stb_image.h"
void framebuffer_size_callback(GLFWwindow *window, int w, int h) {
  glViewport(0, 0, w, h);
}
void process_input(GLFWwindow *window) {
  // Close window on Esc pressed
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
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
  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {  // check
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
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
  stbi_set_flip_vertically_on_load(true);
  int image_w, image_h, image_channel;
  unsigned char *image =
      stbi_load("./opengl_logo.png", &image_w, &image_h, &image_channel, 0);
  if (image) {
    // transfer texture data
    glTexImage2D(GL_TEXTURE_2D,  // texture type
                 0,        // level if this texture is to be part of mipmap
                 GL_RGBA,  // format to store texture
                 image_w, image_h,  // just as is
                 0,        // border, should always be zero for legacy reason
                 GL_RGBA,  // format of input data
                 GL_UNSIGNED_BYTE,  // data layout
                 image              // data
    );  // now the texture has only base-level(0) image attached
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
  float unique_vert[] = {
  // ---- position ----   ----- color -----   - tex coord -
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
  };
  // vertex index for EBO
  GLuint indices[] = {0, 1, 3, 1, 2, 3};
  // clang-format on
  // prepare objects
  GLuint EBO, VAO, VBO;
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  // start to record
  glBindVertexArray(VAO);

  // buffer VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(unique_vert), unique_vert,
               GL_STATIC_DRAW);
  // vertex attrib: position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // vertex attrib: color
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // vertex attrib: tex coord
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // buffer EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);
  // the offset should align with vertex data, not index array
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // finish the recording
  glBindVertexArray(0);

  // render loop
  shader.use();
  // tell the shader which texture unit they are
  shader.set_int("texture0", 0);
  shader.set_int("texture1", 1);

  while (!glfwWindowShouldClose(window)) {
    /**
     * Use color 0.3, 0.3, 0.3 to clear color buffer
     * This is to clear thing in last render loop
     * If called outside the render loop, screen will jitter
     */
    glClearColor(0.3, 0.3, 0.3, 1.0);  // rgba, the value used to clear
    glClear(GL_COLOR_BUFFER_BIT);      // clear the color buffer

    /****** Input ******/
    process_input(window);

    /****** Instruction ******/

    /****** Logic ******/
    glfwPollEvents();


    /****** Render ******/
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glBindVertexArray(VAO);

    // uniform the transform matrix
    glm::mat4 trans{1};
    trans = glm::rotate(trans, float(glfwGetTime()), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::translate(trans, glm::vec3(.5, -.5, .5));
    trans = glm::scale(trans, glm::vec3{0.5, 0.5, 0.5});
    GLuint trans_loc = glGetUniformLocation(shader.ID, "trans");
    glUniformMatrix4fv(trans_loc, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // second
    trans = glm::mat4{1};
    trans = glm::translate(trans, glm::vec3(.5, .5, .5));
    trans = glm::rotate(trans, float(glfwGetTime()), glm::vec3(0.0, 0.0, 1.0));
    trans_loc = glGetUniformLocation(shader.ID, "trans");
    glUniformMatrix4fv(trans_loc, 1, GL_FALSE, glm::value_ptr(trans));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    if (glCheckError() != GL_NO_ERROR) break;
    glfwSwapBuffers(window);  // We use double buffer
  }
  glfwTerminate();
  std::cout << "done\n";
  return 0;
}

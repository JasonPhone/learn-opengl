#include <string>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "learn-opengl/Camera.h"
#include "learn-opengl/Model.h"
#include "learn-opengl/Shader.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// timing
float deltaTime = 0.0f;
float last_frame = 0.0f;

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 10.0f));
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;
bool first_mouse = true;

void framebuffer_size_callback(GLFWwindow *, int w, int h) {
  glViewport(0, 0, w, h);
}
void process_input(GLFWwindow *window) {
  // Close window on Esc pressed
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  // Camera move
  camera.setMoveSpeed(2.5);
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
  camera.turn(xpos * 1.5, ypos * 1.5);
}

void mouse_scroll_callback(GLFWwindow *, double, double yoffset) {
  camera.zoom(yoffset);
}

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  camera.setMoveSpeed(7);

  // glfw window creation
  // --------------------
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwSetScrollCallback(window, mouse_scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }


  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile shaders
  // -------------------------
  Shader model_shader("../shader/model_loading.vs",
                      "../shader/model_loading.fs");

  // load models
  // -----------
  // Model ai_model("../models/cube/cube.obj");
  Model ai_model("../models/nanosuit/nanosuit.obj");
  

  // draw in wireframe
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - last_frame;
    last_frame = currentFrame;

    // input
    // -----
    process_input(window);

    // render
    // ------
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // don't forget to enable shader before setting uniforms
    model_shader.use();

    // view/projection transformations
    glm::mat4 projection =
        glm::perspective(glm::radians(float(camera.fov())),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.viewMatrix();
    model_shader.setMat4fv("projection", glm::value_ptr(projection));
    model_shader.setMat4fv("view", glm::value_ptr(view));

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(
        model,
        glm::vec3(
            0.0f, 0.0f,
            0.0f));  // translate it down so it's at the center of the scene
    model = glm::scale(
        model,
        glm::vec3(1.0f, 1.0f,
                  1.0f));  // it's a bit too big for our scene, so scale it down
    model_shader.setMat4fv("model", glm::value_ptr(model));

    model_shader.setVec3fv("view_pos",
                            glm::value_ptr(camera.cameraPosition()));
    // Light
    float time = glfwGetTime();
    float x = sin(time) * 10;
    float z = cos(time) * 10;
    model_shader.setVec3f("point_light.position", x, 10, z);

    model_shader.setVec3f("point_light.ambient", 0.1f, 0.1f, 0.1f);
    model_shader.setVec3f("point_light.diffuse", 0.8f, 0.8f, 0.8f);
    model_shader.setVec3f("point_light.specular", 1.0f, 1.0f, 1.0f);

    model_shader.setFloat("point_light.att_constant", 1.0f);
    model_shader.setFloat("point_light.att_linear", 0.009f);
    model_shader.setFloat("point_light.att_quadratic", 0.0032f);

    ai_model.draw(model_shader);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

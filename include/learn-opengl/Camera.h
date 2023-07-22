/**
 * @file fps_camera.h
 * @author ja50n (zs_feng@qq.com)
 * @brief A -style camera with functions
 * - W, A, S, D to move camera forward, left, back and right
 * - SPACE, SHIFT to move camera up and down
 * - scroll to zoom in and out
 * @version 0.1
 * @date 2022-09-16
 */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class MOVE_DIRECTION { RIGHT, LEFT, UP, DOWN, FORWARD, BACKWARD };

class Camera {
 public:
  // Camera() = delete;
  // Camera(Camera const &) = delete;
  /**
   * @brief Construct a new Camera object
   *
   * @param pos Position of camera, default (0, 0, 0).
   * @param front Front direction of camera, default (0, 0, -1), will be
   * normalized.
   * @param up Up direction of camera in world coordinates. will be normalized.
   * @param pitch Camera turning up and down, in degree.
   * @param yaw Camera turning left and right, in degree.
   */
  Camera(glm::vec3 pos = glm::vec3{0, 0, 0},
         glm::vec3 front = glm::vec3{0, 0, -1},
         glm::vec3 up = glm::vec3{0, 1, 0}, double pitch = 0, double yaw = 0);
  /**
   * @brief Move the camera by params.
   * The real distance is decided by the delta time and speed.
   *
   * @param dir Direction of this movement, only left, right, up and down.
   * @param delta_time Time duration of this movement.
   */
  void move(MOVE_DIRECTION dir, double delta_time);
  /**
   * @brief Trun the camera direction up-down and left-right.
   * Camera maintains the last position of cursor,
   * and turns the looking direction by the offset with current cursor position.
   *
   * The transforming is based on Euler angles.
   *
   * @param xpos Current X position of cursor.
   * @param ypos Current Y position of cursor.
   */
  void turn(double xpos, double ypos);
  void turnDelta(double xoffset, double yoffset) {
    yoffset = -yoffset;

    float sensitivity = turnSensitivity();
    m_camera_pitch += sensitivity * yoffset;
    m_camera_yaw += sensitivity * xoffset;

    // Restrict the pitch range
    m_camera_pitch = glm::clamp(m_camera_pitch, -89.9, 89.9);

    glm::vec3 front;
    front.x =
        cos(glm::radians(m_camera_pitch)) * sin(glm::radians(m_camera_yaw));
    front.y = sin(glm::radians(m_camera_pitch));
    front.z =
        -cos(glm::radians(m_camera_pitch)) * cos(glm::radians(m_camera_yaw));

    m_cameraFront = front;
    m_camera_right = glm::normalize(glm::cross(m_cameraFront, m_camera_up));
  }
  /**
   * @brief Camera zoom in-out.
   *
   * @param yoffset Delta fov, in degree.
   */
  void zoom(double yoffset);
  /**
   * @brief Get camera field of view in degree
   *
   * @return double
   */
  double fov() const;
  /**
   * @brief Set the move speed.
   *
   * @param spd new speed
   */
  void setMoveSpeed(double spd);
  /**
   * @brief Get the move speed
   *
   * @return double
   */
  double moveSpeed() const;
  /**
   * @brief Set the turn sensitivity.
   *
   * @param sensi New sensitivity.
   */
  void setTurnSensitivity(double sensi);
  /**
   * @brief Get the turn sensitivity
   *
   * @return double
   */
  double turnSensitivity() const;
  /**
   * @brief Get the view matrix for MVP transformation.
   * @return glm::mat4 The view matrix of this camera.
   */
  glm::mat4 viewMatrix() const;
  /**
   * @brief Get current camera position.
   *
   * @return glm::vec3 Position.
   */
  glm::vec3 cameraPosition() const;
  glm::vec3 cameraFront() const;

 private:
  bool m_mouse_first_capture;
  double m_last_xpos, m_last_ypos;
  double m_camera_pitch, m_camera_yaw, m_camera_fov;
  double m_move_speed, m_turn_sensitivity;
  glm::vec3 m_camera_pos;
  glm::vec3 m_cameraFront;
  glm::vec3 m_camera_up;
  glm::vec3 m_camera_right;  // Auxiliary vector
};
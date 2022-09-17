#include "learn-opengl/camera.h"

Camera::Camera(glm::vec3 pos = glm::vec3{0, 0, 0},
               glm::vec3 front = glm::vec3{0, 0, -1},
               glm::vec3 up = glm::vec3{0, 1, 0}, double pitch = 0,
               double yaw = 0)
    : m_camera_pos{pos},
      m_camera_front{glm::normalize(front)},
      m_camera_pitch{pitch},
      m_camera_yaw{yaw} {
  // In case camera up is not perpendicular with look direction
  m_camera_right = glm::normalize(glm::cross(front, up));
  m_camera_up = glm::normalize(glm::cross(m_camera_right, m_camera_front));

  m_mouse_first_capture = true;
  m_move_speed = 0.5;
  m_turn_sensitivity = 0.5;
}

void Camera::move(MOVE_DIRECTION dir, double delta_time) {
  float distance = m_move_speed * delta_time;
  if (dir == MOVE_DIRECTION::FORWARD) {
    m_camera_pos += m_camera_front * distance;
  }
  if (dir == MOVE_DIRECTION::BACKWARD) {
    m_camera_pos -= m_camera_front * distance;
  }
  if (dir == MOVE_DIRECTION::UP) {
    m_camera_pos += m_camera_up * distance;
  }
  if (dir == MOVE_DIRECTION::DOWN) {
    m_camera_pos -= m_camera_up * distance;
  }
  if (dir == MOVE_DIRECTION::RIGHT) {
    m_camera_pos += m_camera_right * distance;
  }
  if (dir == MOVE_DIRECTION::LEFT) {
    m_camera_pos -= m_camera_right * distance;
  }
}

/**
 * @brief Trun the camera direction up-down and left-right.
 * Camera maintains the last position of cursor,
 * and turns the looking direction by the offset with current cursor position.
 *
 * @param xpos Current X position of cursor.
 * @param ypos Current Y position of cursor.
 */
// NOTE The camera vectors need rebuilding after turning
void Camera::turn(double xpos, double ypos) {}

/**
 * @brief Camera zoom in-out.
 *
 * @param yoffset Delta fov, in degree.
 */
void Camera::zoom(double yoffset) {
  m_camera_fov = glm::clamp(m_camera_fov - yoffset, 1.0, 45.0);
}

void Camera::set_move_speed(double spd) {
  m_move_speed = glm::clamp(spd, 0.0, 1e5);
}

void Camera::set_turn_sensitivity(double sensi) {
  m_turn_sensitivity = glm::clamp(sensi, 0.0, 1e5);
}

glm::mat4 Camera::view_matrix() {
  return glm::lookAt(m_camera_pos, m_camera_pos + m_camera_front, m_camera_up);
}
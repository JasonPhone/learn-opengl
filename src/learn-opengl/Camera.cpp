#include "learn-opengl/Camera.h"

#include <glm/gtx/quaternion.hpp>


Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, double pitch,
               double yaw)
    : m_camera_pitch{pitch},
      m_camera_yaw{yaw},
      m_camera_pos{pos},
      m_cameraFront{glm::normalize(front)} {
  // In case camera up is not perpendicular with look direction
  m_camera_right = glm::normalize(glm::cross(front, up));
  m_camera_up = glm::normalize(glm::cross(m_camera_right, m_cameraFront));

  m_mouse_first_capture = true;
  m_camera_fov = 45;
  m_move_speed = 1;
  m_turn_sensitivity = 0.05;
}

void Camera::move(MOVE_DIRECTION direction_flag, double delta_time) {
  float distance = m_move_speed * delta_time;
  if (direction_flag == MOVE_DIRECTION::FORWARD) {
    glm::vec3 dir = glm::normalize(glm::cross(m_camera_up, m_camera_right));
    m_camera_pos += dir * distance;
  }
  if (direction_flag == MOVE_DIRECTION::BACKWARD) {
    glm::vec3 dir = glm::normalize(glm::cross(m_camera_right, m_camera_up));
    m_camera_pos += dir * distance;
  }
  if (direction_flag == MOVE_DIRECTION::UP) {
    m_camera_pos += m_camera_up * distance;
  }
  if (direction_flag == MOVE_DIRECTION::DOWN) {
    m_camera_pos += -m_camera_up * distance;
  }
  if (direction_flag == MOVE_DIRECTION::RIGHT) {
    m_camera_pos += m_camera_right * distance;
  }
  if (direction_flag == MOVE_DIRECTION::LEFT) {
    m_camera_pos += -m_camera_right * distance;
  }
}

void Camera::turn(double xpos, double ypos) {
  if (m_mouse_first_capture) {
    m_mouse_first_capture = false;
    m_last_xpos = xpos;
    m_last_ypos = ypos;
  }
  double xoffset = xpos - m_last_xpos;
  double yoffset = ypos - m_last_ypos;
  yoffset = -yoffset;

  m_last_xpos = xpos;
  m_last_ypos = ypos;

  float sensitivity = turnSensitivity();
  m_camera_pitch += sensitivity * yoffset;
  m_camera_yaw += sensitivity * xoffset;

  // Restrict the pitch range
  m_camera_pitch = glm::clamp(m_camera_pitch, -89.9, 89.9);

  glm::vec3 front;
  front.x = cos(glm::radians(m_camera_pitch)) * sin(glm::radians(m_camera_yaw));
  front.y = sin(glm::radians(m_camera_pitch));
  front.z =
      -cos(glm::radians(m_camera_pitch)) * cos(glm::radians(m_camera_yaw));

  m_cameraFront = front;
  m_camera_right = glm::normalize(glm::cross(m_cameraFront, m_camera_up));
}

void Camera::zoom(double yoffset) {
  m_camera_fov = glm::clamp(m_camera_fov - yoffset, 1.0, 45.0);
}

double Camera::fov() const { return m_camera_fov; }

void Camera::setMoveSpeed(double spd) {
  m_move_speed = glm::clamp(spd, 0.0, 1e5);
}

double Camera::moveSpeed() const { return m_move_speed; }

void Camera::setTurnSensitivity(double sensi) {
  m_turn_sensitivity = glm::clamp(sensi, 0.0, 1e5);
}

double Camera::turnSensitivity() const { return m_turn_sensitivity; }

glm::mat4 Camera::viewMatrix() const {
  return glm::lookAt(m_camera_pos, m_camera_pos + m_cameraFront, m_camera_up);
}

glm::vec3 Camera::cameraPosition() const { return m_camera_pos; }

glm::vec3 Camera::cameraFront() const { return m_cameraFront; }
#include "learn-opengl/camera.h"
#include <glm/gtx/quaternion.hpp>

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, double pitch,
               double yaw)
    : m_camera_pos{pos},
      m_camera_front{glm::normalize(front)},
      m_camera_pitch{pitch},
      m_camera_yaw{yaw} {
  // In case camera up is not perpendicular with look direction
  m_camera_right = glm::normalize(glm::cross(front, up));
  m_camera_up = glm::normalize(glm::cross(m_camera_right, m_camera_front));

  m_mouse_first_capture = true;
  m_camera_fov = 45;
  m_move_speed = 1;
  m_turn_sensitivity = 0.05;
}

void Camera::move(MOVE_DIRECTION dir, double delta_time) {
  float distance = m_move_speed * delta_time;
  if (dir == MOVE_DIRECTION::FORWARD) {
    glm::vec3 dir = glm::normalize(glm::cross(m_camera_up, m_camera_right));
    m_camera_pos += dir * distance;
  }
  if (dir == MOVE_DIRECTION::BACKWARD) {
    glm::vec3 dir = glm::normalize(glm::cross(m_camera_right, m_camera_up));
    m_camera_pos += dir * distance;
  }
  if (dir == MOVE_DIRECTION::UP) {
    m_camera_pos += m_camera_up * distance;
  }
  if (dir == MOVE_DIRECTION::DOWN) {
    m_camera_pos += -m_camera_up * distance;
  }
  if (dir == MOVE_DIRECTION::RIGHT) {
    m_camera_pos += m_camera_right * distance;
  }
  if (dir == MOVE_DIRECTION::LEFT) {
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

  float sensitivity = turn_sensitivity();
  m_camera_pitch += sensitivity * yoffset;
  m_camera_yaw += sensitivity * xoffset;

  // Restrict the pitch range
  m_camera_pitch = glm::clamp(m_camera_pitch, -89.9, 89.9);

  glm::vec3 front;
  front.x = cos(glm::radians(m_camera_pitch)) * sin(glm::radians(m_camera_yaw));
  front.y = sin(glm::radians(m_camera_pitch));
  front.z =
      -cos(glm::radians(m_camera_pitch)) * cos(glm::radians(m_camera_yaw));

  m_camera_front = front;
  m_camera_right = glm::normalize(glm::cross(m_camera_front, m_camera_up));
}

void Camera::zoom(double yoffset) {
  m_camera_fov = glm::clamp(m_camera_fov - yoffset, 1.0, 45.0);
}

double Camera::fov() const { return m_camera_fov; }

void Camera::set_move_speed(double spd) {
  m_move_speed = glm::clamp(spd, 0.0, 1e5);
}

double Camera::move_speed() const { return m_move_speed; }

void Camera::set_turn_sensitivity(double sensi) {
  m_turn_sensitivity = glm::clamp(sensi, 0.0, 1e5);
}

double Camera::turn_sensitivity() const { return m_turn_sensitivity; }

glm::mat4 Camera::view_matrix() const {
  return glm::lookAt(m_camera_pos, m_camera_pos + m_camera_front, m_camera_up);
}

glm::vec3 Camera::camera_position() const { return m_camera_pos; }

glm::vec3 Camera::camera_front() const { return m_camera_front; }
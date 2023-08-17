#include "learn-opengl/Camera.h"

#include <glm/gtx/quaternion.hpp>

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
    : mCameraPos{pos}, mWorldUp{0, 1, 0} {
  mCameraUp = glm::normalize(up);
  front = glm::normalize(front);
  mCameraPitch = glm::degrees(glm::asin(front.y));
  mCameraYaw =
      glm::degrees(glm::acos(front.x / glm::cos(glm::radians(mCameraPitch))));

  updateVectors();

  mCameraFov = 45;
  mMoveSpeed = 1;
  mTurnSeneitivity = 0.05;
}

void Camera::move(MOVE_DIRECTION direction_flag, double delta_time) {
  float distance = mMoveSpeed * delta_time;
  if (direction_flag == MOVE_DIRECTION::FORWARD) {
    glm::vec3 dir = glm::normalize(mCameraFront);
    mCameraPos += dir * distance;
  }
  if (direction_flag == MOVE_DIRECTION::BACKWARD) {
    glm::vec3 dir = -glm::normalize(mCameraFront);
    mCameraPos += dir * distance;
  }
  if (direction_flag == MOVE_DIRECTION::UP) {
    mCameraPos += mWorldUp * distance;
  }
  if (direction_flag == MOVE_DIRECTION::DOWN) {
    mCameraPos += -mWorldUp * distance;
  }
  if (direction_flag == MOVE_DIRECTION::RIGHT) {
    mCameraPos += mCameraRight * distance;
  }
  if (direction_flag == MOVE_DIRECTION::LEFT) {
    mCameraPos += -mCameraRight * distance;
  }
}

void Camera::turn(double xoffset, double yoffset) {
  float sensitivity = turnSensitivity();
  mCameraYaw += sensitivity * xoffset;
  mCameraPitch += sensitivity * yoffset;
  mCameraPitch = glm::clamp(mCameraPitch, -89.9, 89.9);
  updateVectors();
}

void Camera::zoom(double yoffset) {
  mCameraFov = glm::clamp(mCameraFov - yoffset, 1.0, 45.0);
}

double Camera::fov() const { return mCameraFov; }

void Camera::setMoveSpeed(double spd) {
  mMoveSpeed = glm::clamp(spd, 0.0, 1e5);
}

double Camera::moveSpeed() const { return mMoveSpeed; }

void Camera::setTurnSensitivity(double sensi) {
  mTurnSeneitivity = glm::clamp(sensi, 0.0, 1e5);
}

double Camera::turnSensitivity() const { return mTurnSeneitivity; }

glm::mat4 Camera::viewMatrix() const {
  return glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
}

void Camera::updateVectors() {
  // calculate the new Front vector
  glm::vec3 front;
  front.x = cos(glm::radians(mCameraYaw)) * cos(glm::radians(mCameraPitch));
  front.y = sin(glm::radians(mCameraPitch));
  front.z = sin(glm::radians(mCameraYaw)) * cos(glm::radians(mCameraPitch));
  mCameraFront = glm::normalize(front);

  mCameraRight = glm::normalize(glm::cross(mCameraFront, mWorldUp));
  mCameraUp = glm::normalize(glm::cross(mCameraRight, mCameraFront));
}
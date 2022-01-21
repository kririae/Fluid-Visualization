//
// Created by kr2 on 1/13/22.
//

#include "Camera.hpp"

OrbitCamera::OrbitCamera()
    : Position(glm::vec3(0.0f, 3.0f, 6.0f)),
      WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
      Front(glm::vec3(0.0f, -3.0f, -6.0f)), yaw(-120.0f), pitch(-30.0f),
      mouseSensitivity(0.25f)
{
  Front = glm::normalize(Front);
  Right = glm::normalize(glm::cross(Front, WorldUp));
  Up = glm::normalize(glm::cross(Right, Front));

  processDrag(0, 0);
}

glm::mat4
OrbitCamera::getViewMatrix() const
{
  return glm::lookAt(Position, Position + Front, Up);
}

void
OrbitCamera::processDrag(float xoff, float yoff)
{
  yaw += xoff * mouseSensitivity;
  pitch += yoff * mouseSensitivity;

  if (pitch > 89.9f) pitch = 89.9f;
  if (pitch < -89.0f) pitch = -89.0f;

  float distToOrigin = glm::distance(Position, vec3(0.0));
  Position = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                       sin(glm::radians(pitch)),
                       sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
  Position = -distToOrigin * Position;

  updateCameraCoordinate();
}

void
OrbitCamera::processScroll(float yoff)
{
  Position *= (1.0f - yoff * 0.01f);
}

vec3
OrbitCamera::calcScreenSpaceOffset(float xoff, float yoff) const
{
  float distToOrigin = glm::distance(Position, vec3(0.0));
  float sensitivity = 0.001f * distToOrigin;
  xoff *= sensitivity;
  yoff *= sensitivity;
  return yoff * Up + xoff * Right;
}

void
OrbitCamera::updateCameraCoordinate()
{
  Front = glm::normalize(
      glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                sin(glm::radians(pitch)),
                sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
  Right = glm::normalize(glm::cross(Front, WorldUp));
  Up = glm::normalize(glm::cross(Right, Front));
}
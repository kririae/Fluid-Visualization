//
// Created by kr2 on 1/13/22.
//

#ifndef FLUIDVISUALIZATION_SRC_CAMERA_HPP
#define FLUIDVISUALIZATION_SRC_CAMERA_HPP

#include "common.hpp"

class OrbitCamera {
public:
  glm::vec3 Position;
  glm::vec3 WorldUp;
  glm::vec3 Front;
  glm::vec3 Up{};
  glm::vec3 Right{};
  float yaw;
  float pitch;
  float mouseSensitivity;

  OrbitCamera();
  glm::mat4 getViewMatrix() const;
  void processDrag(float xoff, float yoff);
  void processScroll(float yoff);
  vec3 calcScreenSpaceOffset(float xoff, float yoff) const;

private:
  void updateCameraCoordinate();
};

#endif //FLUIDVISUALIZATION_SRC_CAMERA_HPP

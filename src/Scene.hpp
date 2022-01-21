//
// Created by kr2 on 1/14/22.
//

#ifndef FLUIDVISUALIZATION_SRC_SCENE_HPP
#define FLUIDVISUALIZATION_SRC_SCENE_HPP

#include <string>

class Scene {
public:
  explicit Scene(std::string filename);
protected:
  // UI
  int width{1280}, height{720};

  // Solver
  int type{0};
  float border{20}, radius{1.5};
};

#endif //FLUIDVISUALIZATION_SRC_SCENE_HPP

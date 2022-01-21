//
// Created by kr2 on 1/14/22.
//

#include "Scene.hpp"
#include "GUI.hpp"
#include "IncludeSolvers.h"
#include "common.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Scene::Scene(std::string filename)
{
  filename = getPath(filename, 5);

  std::ifstream content(filename);
  json res;
  content >> res;

  // JSON analysis
  if (res.contains("UI")) {
    auto &UI = res["UI"];
    if (UI.contains("width")) { width = UI["width"]; }
    if (UI.contains("height")) { height = UI["height"]; }
  }

  if (res.contains("Solver")) {
    auto &solver = res["Solver"];
    if (solver.contains("type")) { type = solver["type"]; }
    if (solver.contains("border")) { border = solver["border"]; }
    if (solver.contains("radius")) { radius = solver["radius"]; }
  }

  // Init with JSON
  RTGUI gui(width, height);

  std::shared_ptr<Solver> solver;
  switch (type) {
  case 0: solver = makeBasicSPHSolver(border, radius); break;
  case 1: solver = makePBDSolver(border, radius); break;
  default: Assert(false, "Solver type invalid"); break;
  }

  gui.setSolver(solver);

  std::function<void()> callback = [obj = &solver, gui = &gui, &solver] {
    solver->updateGUI(gui);
  };

  for (auto &i : res["Objects"]) {
    // assert(i["type"] == 0);
    if (i["type"] == 0) {
      std::array<int, 3> center = i["center"];
      std::array<int, 3> sideLength = i["sideLength"];
      float coeff = i["coeff"];
      glm::vec3 g_center = glm::vec3(center[0], center[1], center[2]);
      glm::vec3 g_sideLength =
          glm::vec3(sideLength[0], sideLength[1], sideLength[2]);
      glm::vec3 g_corner = g_center - g_sideLength / 2.0f;
      glm::vec3 n_side = g_sideLength / (coeff * radius);

      float inc = coeff * radius;

      for (int x = 0; x < n_side.x; ++x) {
        for (int y = 0; y < n_side.y; ++y) {
          for (int z = 0; z < n_side.z; ++z) {
            glm::vec3 pos = g_corner + glm::vec3(x * inc, y * inc, z * inc);
            solver->addParticle(SPHParticle(pos));
          }
        }
      }
    } else if (i["type"] == 1) {
      std::array<int, 3> center = i["center"];
      int PointNum = i["nParticles"];

      srand(time(0));
      float x, y, z;
      for (int i = 0; i < PointNum; ++i) {
        x = (((float)rand() / (float)RAND_MAX) * 2.0 - 1.0) * border / 1.3;
        y = (((float)rand() / (float)RAND_MAX) * 2.0 - 1.0) * border / 1.3;
        z = (((float)rand() / (float)RAND_MAX) * 2.0 - 1.0) * border / 1.3;
        solver->addParticle(SPHParticle(x, y, z));
      }

    } else {
      Assert(false, "Type error");
    }
  }

  gui.mainLoop(callback);
  gui.del();
}

//
// Created by kr2 on 12/24/21.
//

#ifndef FLUIDVISUALIZATION_SRC_GUI_HPP
#define FLUIDVISUALIZATION_SRC_GUI_HPP

#include "Particle.hpp"
#include "Shader.hpp"
#include "PBuffer.hpp"

#include <functional>
#include <memory>
typedef unsigned int uint;

struct GLFWwindow;
class Solver;
class OrbitCamera;

class GUI {
public:
  GUI(int WIDTH, int HEIGHT);
  GUI(const GUI &) = delete;
  GUI &operator=(const GUI &) = delete;
  virtual ~GUI() = default;
  virtual void mainLoop(const std::function<void()> &callback);

protected:
  GLFWwindow *window{};
  int width, height;
};

class RTGUI : public GUI {
  // REAL-TIME GUI for Lagrange View stimulation(particles)

public:
  RTGUI(int WIDTH, int HEIGHT);
  ~RTGUI() override = default;

  void setParticles(const PBuffer& _buffer);
  void setSolver(std::shared_ptr<Solver> _solver);
  void mainLoop(const std::function<void()> &callback) override;
  void del();

protected:
  PBuffer buffer;

  uint frame = 0;
  unsigned int VAO{}, VBO{}, EBO{};
  std::unique_ptr<Shader> p_shader{}, m_shader{};

  std::shared_ptr<hvector<vec3> > mesh;
  std::shared_ptr<hvector<uint> > indices;
  std::shared_ptr<Solver> solver;
  std::shared_ptr<OrbitCamera> camera;

  bool snapshot{false};
  bool pause{false};
  uint lastPauseFrame{};

  float lastX;
  float lastY;
  bool firstMouse{false};

private:
  void renderParticles() const;
  void refreshFps() const;
  void processInput();
  void saveParticles() const;
  void mouseCallback(double x, double y);
};

#endif //FLUIDVISUALIZATION_SRC_GUI_HPP

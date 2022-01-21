//
// Created by kr2 on 12/24/21.
//

#ifndef FLUIDVISUALIZATION_SRC_SOLVER_HPP
#define FLUIDVISUALIZATION_SRC_SOLVER_HPP

#include "PBuffer.hpp"

#include <memory>

class RTGUI;
class SPHParticle;

class Solver {
public:
  Solver(float border, float radius, float epsilon);
  Solver(const Solver &solver) = delete;
  Solver &operator=(const Solver &solver) = delete;
  virtual ~Solver() = default;

  virtual void updateGUI(RTGUI *gui_ptr) = 0;
  virtual void subStep() = 0;
  virtual void addParticle(const SPHParticle &p) = 0;

  virtual float getBorder();
  virtual float getRadius();

protected:
  int n_substeps = 1;
  float border, radius, radius2, epsilon;
};

class ExampleSolver : public Solver {
public:
  ExampleSolver(float border, float radius, float epsilon = 1e-5);
  ExampleSolver(const ExampleSolver &solver) = delete;
  ExampleSolver &operator=(const ExampleSolver &solver) = delete;
  ~ExampleSolver() override = default;

  void updateGUI(RTGUI *gui_ptr) override;
  void subStep() override;
  void addParticle(const SPHParticle &p) override;

protected:
  PBuffer buffer;
};

std::shared_ptr<Solver>
makeExampleSolver(float border, float radius, float epsilon = 1e-5);

#endif //FLUIDVISUALIZATION_SRC_SOLVER_HPP

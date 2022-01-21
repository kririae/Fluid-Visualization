//
// Created by kr2 on 1/14/22.
//

#ifndef FLUIDVISUALIZATION_SRC_BASICSPHSOLVER_HPP
#define FLUIDVISUALIZATION_SRC_BASICSPHSOLVER_HPP

#include "Solver.hpp"

class RTGUI;
class NSearch;

class BasicSPHSolver : public Solver {
public:
  BasicSPHSolver(float border, float radius, float epsilon = 1e-5);
  BasicSPHSolver(const BasicSPHSolver &solver) = delete;
  BasicSPHSolver &operator=(const BasicSPHSolver &solver) = delete;
  ~BasicSPHSolver() override = default;

  void updateGUI(RTGUI *gui_ptr) override;
  void subStep() override;
  void addParticle(const SPHParticle &p) override;

protected:
  float rho_0 = 1.0f; // reference density
  float k = 1000.0f; // stiffness parameter
  float gamma = 7.0f; // parameter
  float mass = 0.0f;
  float delta_t{ 1 / 60.0f };
  int iter = 20;

  float viscosityCoefficient = 0.01f;

  vec3 extF = vec3(0.0f, -9.8f, 0.0f);

  std::shared_ptr<NSearch> nsearch;
  PBuffer buffer;

  void constraintToBorder(SPHParticle &p);
  static float poly6(float r, float d) noexcept;
  static vec3 gradSpiky(vec3 v, float d) noexcept;
  float sphCalcRho(int p_i);
};

std::shared_ptr<Solver> makeBasicSPHSolver(float border, float radius,
                                           float epsilon = 1e-5);

#endif //FLUIDVISUALIZATION_SRC_BASICSPHSOLVER_HPP

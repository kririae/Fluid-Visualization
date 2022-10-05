//
// Created by kr2 on 12/25/21.
//

#ifndef FLUIDVISUALIZATION_SRC_PBDSOLVERCPU_HPP
#define FLUIDVISUALIZATION_SRC_PBDSOLVERCPU_HPP

#include "Solver.hpp"

class NSearch;

class PBDSolver : public Solver {
public:
  PBDSolver(float border, float radius, float epsilon = 1e-5);
  PBDSolver(const PBDSolver &solver) = delete;
  PBDSolver &operator=(const PBDSolver &solver) = delete;
  ~PBDSolver() override = default;

  void updateGUI(RTGUI *gui_ptr) override;
  void subStep() override;
  void addParticle(const SPHParticle &p) override;

protected:
  PBuffer buffer;
  std::shared_ptr<NSearch> nsearch;

  float rho_0 = 1057.434f;
  float denominatorEpsilon = 170.0f;
  float delta_t{ 1 / 60.0f };

  // Mostly from 0 to 1
  float viscosityCoefficient = 0.0001f;

  float mass;
  int iter = 2;
  vec3 extF = vec3(0.0f, -9.8f, 0.0f);

  void constraintToBorder(SPHParticle &p);

  // PBF mathematics
  static float poly6(float r, float d) noexcept;
  static vec3 gradSpiky(vec3 v, float d) noexcept;
  inline float computeSCorr(vec3 p_i, vec3 p_j);
};

std::shared_ptr<Solver> makePBDSolver(float border, float radius,
                                      float epsilon = 1e-5);

#endif //FLUIDVISUALIZATION_SRC_PBDSOLVERCPU_HPP

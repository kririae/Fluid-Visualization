//
// Created by kr2 on 1/14/22.
//

#include "BasicSPHSolver.hpp"
#include "GUI.hpp"
#include "NSearch.hpp"
#include "Solver.hpp"
#include "common.hpp"

#include <chrono>

BasicSPHSolver::BasicSPHSolver(float border, float radius, float epsilon)
    : Solver(border, radius, epsilon)
{
  buffer = makePBuffer();
  // nsearch = makeNSearch(border, radius);
  nsearch = makeNSearchExt(border, radius);
  mass = 2.0f;
  n_substeps = 20;
}

void
BasicSPHSolver::updateGUI(RTGUI *gui_ptr)
{
  delta_t /= n_substeps;
  for (int i = 0; i < n_substeps; ++i)
    this->subStep();
  delta_t *= n_substeps;
  gui_ptr->setParticles(buffer);
}

void
BasicSPHSolver::subStep()
{
  static int interval = 60;
  static int nNeighborSum = 0;
  static float rhoSum = 0.0f;

  auto &data = *buffer;
  const int dataSize = data.size();

  // find all neighbors
  auto dt_start = std::chrono::system_clock::now();
  nsearch->setBuffer(buffer);
  nsearch->build();
  auto dt_end = std::chrono::system_clock::now();

  auto start = std::chrono::system_clock::now();
  std::vector<float> rho(dataSize), p(dataSize);
  std::vector<glm::vec3> force(dataSize);
  std::vector<vec3> XSPHViscosity(dataSize);
#pragma omp parallel for default(none) shared(dataSize, force, rho, p, gamma)
  for (int i = 0; i < dataSize; ++i) {
    rho[i] = sphCalcRho(i);
    p[i] = k * rho_0 / gamma * (fastPow(rho[i] / rho_0, gamma) - 1);
  }

#pragma omp parallel for default(none) \
    shared(dataSize, rho, p, gamma, nNeighborSum, force, data, rhoSum, XSPHViscosity)
  for (int i = 0; i < dataSize; ++i) {
    glm::vec3 v_xsph(0.0f);

    int nNeighbor = nsearch->nNeighbor(i);
    nNeighborSum += nNeighbor;
    for (int j = 0; j < nNeighbor; ++j) {
      const int neighbor_index = nsearch->neighbor(i, j);
      const float tmp =
          p[i] / (rho[i] * rho[i]) +
          p[neighbor_index] / (rho[neighbor_index] * rho[neighbor_index]);
      force[i] +=
          tmp * gradSpiky(data[i].pos - data[neighbor_index].pos, radius);

      v_xsph += viscosityCoefficient * (data[neighbor_index].v - data[i].v) *
          poly6(glm::length(data[i].pos - data[neighbor_index].pos), radius);
    }

    force[i] *= (-mass * mass);
    data[i].rho = rho[i];
    XSPHViscosity[i] = v_xsph;
    rhoSum += rho[i];
    force[i] += extF; // Apply external force
  }

  for (int i = 0; i < dataSize; ++i) {
    auto &particle = data[i];
    particle.v += delta_t * force[i] / mass + XSPHViscosity[i];
    particle.pos += delta_t * particle.v;
    constraintToBorder(particle);
  }

  if ((--interval) != 0) return;

  interval = 60;
  std::cout << "--- callback start (interval: 60) ---" << std::endl;
  std::cout << "NParticles: " << dataSize << std::endl;
  std::chrono::duration<float> dt_diff = dt_end - dt_start;
  std::cout << "Data structure building complete: " << dt_diff.count() * 1000
            << "ms" << std::endl;
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<float> diff = end - start;
  std::cout << "Calculation complete: " << diff.count() * 1000 << "ms"
            << std::endl;
  std::cout << "Avg nNeighbor: " << nNeighborSum / dataSize / interval
            << std::endl;
  std::cout << "Avg rho: " << rhoSum / dataSize / interval << std::endl;
  nNeighborSum = 0;
  rhoSum = 0.0f;
}

void
BasicSPHSolver::addParticle(const SPHParticle &p)
{
  assert(buffer != nullptr);
  buffer->push_back(p);
}

void
BasicSPHSolver::constraintToBorder(SPHParticle &p)
{
  float minVal = -border * 0.99f, maxVal = border * 0.99f;
  p.pos.x = glm::clamp(p.pos.x, minVal, maxVal);
  p.pos.y = glm::clamp(p.pos.y, minVal, maxVal);
  p.pos.z = glm::clamp(p.pos.z, minVal, maxVal);
  if (p.pos.x == minVal || p.pos.x == maxVal) p.v.x = 0;
  if (p.pos.y == minVal || p.pos.y == maxVal) p.v.y = 0;
  if (p.pos.z == minVal || p.pos.z == maxVal) p.v.z = 0;
}

float
BasicSPHSolver::poly6(float r, float d) noexcept
{
  r = glm::clamp(glm::abs(r), 0.0f, d);
  const float t = (d * d - r * r) / (d * d * d);
  return 315.0f / (64 * glm::pi<float>()) * t * t * t;
}

vec3
BasicSPHSolver::gradSpiky(vec3 v, float d) noexcept
{
  float len = glm::length(v);
  vec3 res(0.0f);
  if (0 < len && len <= d)
    res =
        float(-45 / (glm::pi<float>() * fastPow(d, 6)) * fastPow(d - len, 2)) *
        v / len;
  return res;
}

float
BasicSPHSolver::sphCalcRho(int p_i)
{
  float rho = 0;
  const auto &data = *buffer;
  for (int i = 0; i < nsearch->nNeighbor(p_i); ++i) {
    const int neighbor_index = nsearch->neighbor(p_i, i);
    rho += mass *
           poly6(glm::length(data[p_i].pos - data[neighbor_index].pos), radius);
  }

  return rho;
}

std::shared_ptr<Solver>
makeBasicSPHSolver(float border, float radius, float epsilon)
{
  return static_cast<const std::shared_ptr<Solver> &>(
      std::make_shared<BasicSPHSolver>(border, radius, epsilon));
}

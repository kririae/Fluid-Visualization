//
// Created by kr2 on 12/25/21.
//

#include "PBDSolverCPU.hpp"
#include "GUI.hpp"
#include "NSearch.hpp"
#include "common.hpp"

#include <chrono>

PBDSolver::PBDSolver(float _border, float _radius, float _epsilon)
    : Solver(_border, _radius, _epsilon)
{
  // Do nothing
  buffer = makePBuffer();
  nsearch = makeNSearch(border, radius);
  // nsearch = makeNSearchExt(border, radius);
  // mass = 4.0f / 3.0f * glm::pi<float>() * radius2 * radius;
  mass = 2.0f;
}

void
PBDSolver::updateGUI(RTGUI *gui_ptr)
{
  for (int i = 0; i < n_substeps; ++i)
    this->subStep();
  gui_ptr->setParticles(buffer);
}

void
PBDSolver::addParticle(const SPHParticle &p)
{
  assert(buffer != nullptr);
  buffer->push_back(p);
}

void
PBDSolver::subStep()
{
  static int interval = 60;

  auto &data = *buffer;
  const auto pre_data = data; // copy
  const int dataSize = data.size();

  // Apply forces
  for (auto &i : data) {
    i.v += delta_t * extF / mass;
    i.pos += delta_t * i.v;
    constraintToBorder(i);
  }

  // find all neighbors
  auto dt_start = std::chrono::system_clock::now();
  nsearch->setBuffer(buffer);
  nsearch->build();
  auto dt_end = std::chrono::system_clock::now();

  auto start = std::chrono::system_clock::now();
  // Jacobi iteration
  double rhoSum = 0;
  long long nNeighborSum = 0;

  std::vector<float> _lambda(dataSize), c_i(dataSize);
  std::vector<vec3> XSPHViscosity(dataSize);

  int iter_cnt = iter;
  while (iter_cnt--) {
#pragma omp parallel for default(none) \
    shared(dataSize, c_i, rhoSum, nNeighborSum, _lambda)
    for (int i = 0; i < dataSize; ++i) {
      // Basic calculation
      const float rho = sphCalcRho(i);
      c_i[i] = rho / rho_0 - 1;
      rhoSum += rho;

      float _denom = 0.0f;
      const int nNeighbor = nsearch->nNeighbor(i);
      nNeighborSum += nNeighbor;

      for (int j = 0; j < nNeighbor; ++j)
        _denom += fastPow(glm::length(gradC(i, nsearch->neighbor(i, j))), 2.0f);

      _lambda[i] = -c_i[i] / (_denom + denominatorEpsilon);
    }

#pragma omp parallel for default(none) \
    shared(dataSize, _lambda, data, c_i, XSPHViscosity)
    for (int i = 0; i < dataSize; ++i) {
      vec3 delta_p_i(0.0f), v_xsph(0.0f);

      const int nNeighbor = nsearch->nNeighbor(i);
      for (int j = 0; j < nNeighbor; ++j) {
        int neighbor_index = nsearch->neighbor(i, j);

        float r = glm::length(data[i].pos - data[neighbor_index].pos);
        delta_p_i += (_lambda[i] + _lambda[neighbor_index] +
                      computeSCorr(i, neighbor_index)) *
                     gradSpiky(data[i].pos - data[neighbor_index].pos, radius);
        v_xsph += viscosityCoefficient * (data[neighbor_index].v - data[i].v) *
                  poly6(r, radius);
      }

      delta_p_i *= 1.0f / rho_0;
      data[i].pos += delta_p_i;
      data[i].rho = glm::clamp((c_i[i] + 1) * rho_0, 0.0f, 1.0f);
      XSPHViscosity[i] = v_xsph;

      constraintToBorder(data[i]);
    }
  }

  // update all velocity
  for (int i = 0; i < dataSize; ++i) {
    auto &p = data[i];
    p.v = 1.0f / delta_t * (p.pos - pre_data[i].pos) + XSPHViscosity[i];
  }

  // Logging part
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
  std::cout << "Avg rho: " << rhoSum / dataSize / iter << " | nNeighbor: "
            << static_cast<float>(nNeighborSum) / dataSize / iter << std::endl;
  std::cout << std::endl;
}

float
PBDSolver::sphCalcRho(int p_i)
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

vec3
PBDSolver::gradC(int p_i, int p_k)
{
  const auto &data = *buffer;
  vec3 res(0.0f);
  if (p_i == p_k) {
    for (int i = 0; i < nsearch->nNeighbor(p_i); ++i) {
      const int neighbor_index = nsearch->neighbor(p_i, i);
      res += gradSpiky(data[p_i].pos - data[neighbor_index].pos, radius);
    }
  } else {
    res = -gradSpiky(data[p_i].pos - data[p_k].pos, radius);
  }

  return 1.0f / rho_0 * res;
}

float
PBDSolver::poly6(float r, float d) noexcept
{
  r = glm::clamp(glm::abs(r), 0.0f, d);
  const float t = (d * d - r * r) / (d * d * d);
  return 315.0f / (64 * glm::pi<float>()) * t * t * t;
}

vec3
PBDSolver::gradSpiky(vec3 v, float d) noexcept
{
  float len = glm::length(v);
  vec3 res(0.0f);
  if (0 < len && len <= d)
    res =
        float(-45 / (glm::pi<float>() * fastPow(d, 6)) * fastPow(d - len, 2)) *
        v / len;
  return res;
}

inline float
PBDSolver::computeSCorr(int p_i, int p_j)
{
  float k = 0.1f;
  float n = 4.0f;
  float delta_q = 0.3f * radius;
  const auto &data = *buffer;
  float r = glm::length(data[p_i].pos - data[p_j].pos);
  return -k * fastPow(poly6(r, radius) / poly6(delta_q, radius), n);
}

void
PBDSolver::constraintToBorder(SPHParticle &p)
{
  p.pos.x = glm::clamp(p.pos.x, -border * 0.99f, border * 0.99f);
  p.pos.y = glm::clamp(p.pos.y, -border * 0.99f, border * 0.99f);
  p.pos.z = glm::clamp(p.pos.z, -border * 0.99f, border * 0.99f);
}

std::shared_ptr<Solver>
makePBDSolver(float border, float radius, float epsilon)
{
  return static_cast<const std::shared_ptr<Solver> &>(
      std::make_shared<PBDSolver>(border, radius, epsilon));
}

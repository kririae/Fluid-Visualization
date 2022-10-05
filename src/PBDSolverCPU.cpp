//
// Created by kr2 on 12/25/21.
//

#include "PBDSolverCPU.hpp"
#include "GUI.hpp"
#include "NSearch.hpp"
#include "common.hpp"

#include <chrono>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>

PBDSolver::PBDSolver(float _border, float _radius, float _epsilon)
    : Solver(_border, _radius, _epsilon)
{
  // Do nothing
  buffer = makePBuffer();
  // nsearch = makeNSearch(border, radius);
  nsearch = makeNSearchExt(border, radius);
  mass = 1.0f;
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

  static constexpr std::size_t padding = 1;
  std::vector<float> _lambda(dataSize * padding), c_i(dataSize * padding);
  std::vector<vec3> XSPHViscosity(dataSize * padding);

  int iter_cnt = iter;
  while (iter_cnt--) {
#pragma omp parallel for schedule(static)
    for (int i = 0; i < dataSize; ++i) {
      // INPUT
      float density = 0.0, denominator = 0.0;
      int nNeighbor = nsearch->nNeighbor(i);
      vec3 localPosition = data[i].pos, c_ii = glm::zero<vec3>();

      // First traversal phase
      for (int j = 0; j < nNeighbor; ++j) {
        int neighborIndex = nsearch->neighbor(i, j);
        vec3 neighborPosition = data[neighborIndex].pos;
        vec3 deltaPosition = localPosition - neighborPosition;
        density += mass * poly6(glm::length(deltaPosition), radius);
        c_ii += gradSpiky(deltaPosition, radius);

        // if k == j
        if (neighborIndex != i) {
          vec3 grad = gradSpiky(deltaPosition, radius);
          denominator += glm::dot(grad, grad);
        }
      }

      denominator += glm::dot(c_ii, c_ii);
      denominator /= (rho_0 * rho_0);

      // OUTPUT
      c_i[i * padding] = density / rho_0 - 1;
      _lambda[i * padding] =
          -c_i[i * padding] / (denominator + denominatorEpsilon);
    }

#pragma omp parallel for schedule(static)
    for (int i = 0; i < dataSize; ++i) {
      vec3 deltaP(0.0f), vXSPH(0.0f);
      vec3 localPosition = data[i].pos;
      vec3 localVelocity = data[i].v;

      const int nNeighbor = nsearch->nNeighbor(i);
      for (int j = 0; j < nNeighbor; ++j) {
        int neighborIndex = nsearch->neighbor(i, j);
        vec3 neighborPosition = data[neighborIndex].pos;
        vec3 neighborVelocity = data[neighborIndex].v;
        vec3 deltaPosition = localPosition - neighborPosition;

        float r = glm::length(deltaPosition);
        deltaP += (_lambda[i * padding] + _lambda[neighborIndex * padding] +
                   computeSCorr(localPosition, neighborPosition)) *
                  gradSpiky(deltaPosition, radius);
        vXSPH += viscosityCoefficient * (neighborVelocity - localVelocity) *
                 poly6(r, radius);
      }

      deltaP *= 1.0f / rho_0;
      data[i].pos += deltaP;
      data[i].rho = glm::clamp((c_i[i * padding] + 1) * rho_0, 0.0f, 1.0f);
      XSPHViscosity[i * padding] = vXSPH;

      constraintToBorder(data[i]);
    }
  }

  // update all velocity
  for (int i = 0; i < dataSize; ++i) {
    auto &p = data[i];
    p.v =
        1.0f / delta_t * (p.pos - pre_data[i].pos) + XSPHViscosity[i * padding];
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
PBDSolver::poly6(float r, float d) noexcept
{
  // r = glm::clamp(glm::abs(r), 0.0f, d);
  if (0 <= r && r < d) {
    const float t = (d * d - r * r) / (d * d * d);
    return 315.0f / (64 * glm::pi<float>()) * t * t * t;
  } else {
    return 0.0f;
  }
}

vec3
PBDSolver::gradSpiky(vec3 v, float d) noexcept
{
  float len = glm::length(v);
  if (0 < len && len < d) {
    return float(-45.0 / (glm::pi<float>() * fastPow(d, 6)) *
                 fastPow(d - len, 2)) *
           v / len;
  } else {
    return glm::zero<vec3>();
  }
}

inline float
PBDSolver::computeSCorr(vec3 p_i, vec3 p_j)
{
  float k = 0.001f;
  float n = 2.0f;
  float delta_q = 0.4f * radius;
  float r = glm::length(p_i - p_j);
  return -k * fastPow(poly6(r, radius) / poly6(delta_q, radius), n);
}

void
PBDSolver::constraintToBorder(SPHParticle &p)
{
  p.pos.x = glm::clamp(p.pos.x, -border * 0.9f, border * 0.9f);
  p.pos.y = glm::clamp(p.pos.y, -border * 0.9f, border * 0.9f);
  p.pos.z = glm::clamp(p.pos.z, -border * 0.9f, border * 0.9f);
  if (p.pos.x == -border * 0.9f || p.pos.x == border * 0.9f) p.v.x = 0.0;
  if (p.pos.y == -border * 0.9f || p.pos.y == border * 0.9f) p.v.y = 0.0;
  if (p.pos.z == -border * 0.9f || p.pos.z == border * 0.9f) p.v.z = 0.0;
}

std::shared_ptr<Solver>
makePBDSolver(float border, float radius, float epsilon)
{
  return static_cast<const std::shared_ptr<Solver> &>(
      std::make_shared<PBDSolver>(border, radius, epsilon));
}

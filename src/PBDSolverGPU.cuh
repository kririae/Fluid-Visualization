//
// Created by kr2 on 12/25/21.
//

#ifndef FLUIDVISUALIZATION_SRC_PBDSolverGPUGPU_CUH
#define FLUIDVISUALIZATION_SRC_PBDSolverGPUGPU_CUH

#include "Solver.hpp"
#include <memory>
#include <vector>

class SPHParticle;
class RTGUI;

class PBDSolverGPU : public Solver {
  // Stateful solver
public:
  static constexpr int MAX_NEIGHBOR_SIZE = 60;

  // modifiable value on IMGUI
  float rho_0 = 8.0f;
  int iter = 5;
  vec3 ext_f = vec3(0.0f, -9.8f, 0.0f);

  PBDSolverGPU(float border, float radius, float epsilon = 1e-5);
  PBDSolverGPU(const PBDSolverGPU &solver) = delete;
  PBDSolverGPU &operator=(const PBDSolverGPU &solver) = delete;
  ~PBDSolverGPU() override = default;

  void updateGUI(RTGUI *gui_ptr) noexcept override;
  void subStep() override;
  void addParticle(const SPHParticle &p) override;

  // PBF mathematics parts...
  CUDA_FUNC_DEC static float poly6(float r, float h) noexcept;
  CUDA_FUNC_DEC static vec3 gradSpiky(vec3 v, float h) noexcept;
  CUDA_FUNC_DEC static float computeSCorr(const SPHParticle &p_i,
                                            const SPHParticle &p_j, float h);
  __attribute__((device)) static void constraintToBorder(SPHParticle &p);

  // clang-format off
  // hash function
  [[nodiscard]] static CUDA_FUNC_DEC inline int hash(
    float x,
    float y,
    float z,
    int n_grids
  );

  [[nodiscard]] static CUDA_FUNC_DEC inline int hash(const vec3 &p,
                                                     int n_grids);
  [[nodiscard]] static CUDA_FUNC_DEC inline int hash_from_grid(
    int u,
    int v,
    int w,
    int n_grids
  );

  [[nodiscard]] static CUDA_FUNC_DEC inline int hash_from_grid(const ivec3 &p,
                                                               int n_grids);
  [[nodiscard]] static CUDA_FUNC_DEC inline ivec3 get_grid_index(const vec3 &p);

 private:
  // RTGUI_particles *gui_ptr{nullptr};
  float radius, radius2, mass{0}, delta_t{1.0f / 60.0f / n_substeps};
  int n_grids;
  hvector<SPHParticle> data;
};

__attribute__((global)) void update_velocity(
  SPHParticle *dev_data,
  SPHParticle *pre_data,
  int data_size,
  float delta_t
);

__attribute__((global)) void apply_force(
  SPHParticle *dev_data,
  int data_size,
  vec3 ext_f,
  float delta_t
);

__attribute__((global)) void fill_lambda(
  SPHParticle *dev_data,
  size_t data_size,
  int *dev_neighbor_map,
  const int *dev_n_neighbor_map,
  size_t pitch_neighbor,
  float *c_i,
  float *lambda,
  float rho_0,
  float mass
);

__attribute__((global)) void apply_motion(
  SPHParticle *dev_data,
  size_t data_size,
  const int *dev_n_neighbor_map,
  const int *dev_neighbor_map,
  const float *lambda,
  const float *c_i,
  size_t pitch,
  float rho_0
);

// Allow access to hash function
__attribute__((global)) void build_hash_map(
  SPHParticle *dev_data,
  int data_size,
  int *dev_hash_map,
  int *dev_n_hash_map,
  int n_grids,
  size_t pitch_hash,
  int *hash_map_mutex
);

__attribute__((global)) void build_neighbor_map(
  SPHParticle *dev_data,
  int data_size,
  int *dev_neighbor_map,
  int *dev_n_neighbor_map,
  const int *dev_hash_map,
  const int *dev_n_hash_map,
  int n_grids,
  size_t pitch_neighbor,
  size_t pitch_hash
);

// clang-format on

#endif //FLUIDVISUALIZATION_SRC_PBDSolverGPUGPU_CUH

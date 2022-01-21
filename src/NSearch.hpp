//
// Created by kr2 on 12/25/21.
//

#ifndef FLUIDVISUALIZATION_SRC_NSEARCH_HPP
#define FLUIDVISUALIZATION_SRC_NSEARCH_HPP

#include "PBuffer.hpp"
#include "Particle.hpp"

#include <CompactNSearch>

typedef unsigned int uint;

constexpr uint MAX_NEIGHBOR_SIZE = 60;

// CPU Version
class NSearch {
  // Currently, a poor implementation (for correctness)
public:
  explicit NSearch(float _border, float _radius);
  NSearch(const NSearch &CH) = delete;
  NSearch &operator=(const NSearch &CH) = delete;
  ~NSearch() = default;

  // interface
  virtual void build();

  // Call `build` after you setBuffer
  virtual void setBuffer(PBuffer buffer);

  // Get the number of points managed by NSearch
  [[nodiscard]] virtual int nPoints() const;

  // The number of neighbors around the index
  [[nodiscard]] virtual int nNeighbor(uint index) const;

  // The get specific index by `neighbor_index`, 0 <= neighbor_index <= nNeighbor(index)
  [[nodiscard]] virtual int neighbor(uint index, uint neighbor_index) const;

  std::vector<uint> &neighborVec(uint index);
  PBuffer getBuffer();

protected:
  float radius, radius2, border;

private:
  int n_grids;
  PBuffer buffer;
  std::vector<std::vector<uint> > neighborMap{};
  std::vector<std::vector<int> > hashMap{};

  // hash function
  [[nodiscard]] inline int hash(float x, float y, float z) const;
  [[nodiscard]] inline int hash(const vec3 &p) const;
  [[nodiscard]] inline int hashFromGrid(int u, int v, int w) const;
  [[nodiscard]] inline int hashFromGrid(const ivec3 &p) const;
  [[nodiscard]] inline ivec3 getGridIndex(const vec3 &p) const;
};

class NSearchExt : public NSearch {
  // `CompactNSearch` wrapper for NSearch
public:
  explicit NSearchExt(float _border, float _radius);
  NSearchExt(const NSearchExt &CH) = delete;
  NSearchExt &operator=(const NSearchExt &CH) = delete;
  ~NSearchExt() = default;

  void build() override;
  void setBuffer(PBuffer buffer) override;
  [[nodiscard]] int nPoints() const override;
  [[nodiscard]] int nNeighbor(uint index) const override;
  [[nodiscard]] int neighbor(uint index, uint neighbor_index) const override;

protected:
  uint pointSetID{};
  CompactNSearch::NeighborhoodSearch nsearch;
  std::unique_ptr<CompactNSearch::PointSet> pointSet;

private:
  std::vector<std::array<CompactNSearch::Real, 3> > positions;
};

std::shared_ptr<NSearch>
makeNSearch(float border, float radius);

std::shared_ptr<NSearch>
makeNSearchExt(float border, float radius);

#endif //FLUIDVISUALIZATION_SRC_NSEARCH_HPP

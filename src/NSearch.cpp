//
// Created by kr2 on 12/25/21.
//

#include "NSearch.hpp"
#include "common.hpp"

#include <algorithm>
#include <iostream>
#include <array>
// #include <parallel/algorithm>
#include <utility>

typedef unsigned int uint;
typedef unsigned long ulong;

NSearch::NSearch(float _border, float _radius)
    : radius(_radius), radius2(radius * radius), border(_border),
      n_grids(int(glm::ceil(2 * _border / radius) + 1))
{
  // Initialize to index sort
  hashMap = std::vector<std::vector<int> >(n_grids * n_grids * n_grids);
}

void
NSearch::setBuffer(PBuffer _buffer)
{
  buffer = std::move(_buffer);
}

PBuffer
NSearch::getBuffer()
{
  return buffer;
}

int
NSearch::nPoints() const
{
  return int(buffer->size());
}

int
NSearch::nNeighbor(uint index) const
{
  return int(neighborMap[index].size());
}

int
NSearch::neighbor(uint index, uint neighbor_index) const
{
  return int(neighborMap[index][neighbor_index]);
}

void
NSearch::build()
{
  neighborMap = std::vector<std::vector<uint> >(nPoints());

  auto data = *buffer;
  const int dataSize = int(data.size());

  // Clear previous information
  std::for_each(hashMap.begin(), hashMap.end(), [&](auto &i) { i.clear(); });

  // Initialize the hash_map
  for (int i = 0; i < dataSize; ++i) {
    const int hash_map_index = hash(data[i].pos);
    Assert(hash_map_index >= 0, "index must be positive");
    hashMap[hash_map_index].push_back(i);
  }

#pragma omp parallel for default(none) shared(dataSize, n_grids, data)
  for (int i = 0; i < dataSize; ++i) {
    const auto &center = data[i];
    const auto &grid_index = getGridIndex(center.pos);

    for (int u = grid_index.x - 1; u <= grid_index.x + 1; ++u) {
      for (int v = grid_index.y - 1; v <= grid_index.y + 1; ++v) {
        for (int w = grid_index.z - 1; w <= grid_index.z + 1; ++w) {
          if (u < 0 || v < 0 || w < 0 || u >= n_grids || v >= n_grids ||
              w >= n_grids)
            continue;

          const int _hash_index = hashFromGrid(u, v, w);
          const std::vector<int> &map_item = hashMap[_hash_index];
          std::for_each(map_item.cbegin(), map_item.cend(), [&](int j) {
            if (i != j && center.dist2(data[j]) <= radius2 &&
                neighborMap[i].size() <= ulong(MAX_NEIGHBOR_SIZE))
              neighborMap[i].push_back(j);
          });
        }
      }
    }
  }
}

inline int
NSearch::hash(float x, float y, float z) const
{
  const auto &grid_index = getGridIndex(vec3(x, y, z));
  return hashFromGrid(grid_index);
}

inline int
NSearch::hash(const vec3 &p) const
{
  return hash(p.x, p.y, p.z);
}

inline ivec3
NSearch::getGridIndex(const vec3 &p) const
{
  int u = (int)(glm::floor((p.x + border) / radius));
  int v = (int)(glm::floor((p.y + border) / radius));
  int w = (int)(glm::floor((p.z + border) / radius));
  return { u, v, w };
}

inline int
NSearch::hashFromGrid(int u, int v, int w) const
{
  return u + v * n_grids + w * n_grids * n_grids;
}

inline int
NSearch::hashFromGrid(const ivec3 &p) const
{
  return hashFromGrid(p.x, p.y, p.z);
}

std::vector<uint> &
NSearch::neighborVec(uint index)
{
  // NOT APPRECIATED
  return neighborMap[index];
}

NSearchExt::NSearchExt(float _border, float _radius)
    : NSearch(_border, _radius), nsearch(_radius, true)
{
}

void
NSearchExt::build()
{
  nsearch.find_neighbors();
  pointSet =
      std::make_unique<CompactNSearch::PointSet>(nsearch.point_set(pointSetID));
}

void
NSearchExt::setBuffer(PBuffer buffer)
{
  const int bufferSize = buffer->size();
  if (positions.empty()) {
    // For the first time, initialize the settings
    for (int i = 0; i < bufferSize; ++i) {
      const auto element = (*buffer)[i];
      positions.push_back(std::array<CompactNSearch::Real, 3>{
          element.pos.x, element.pos.y, element.pos.z });
    }

    pointSetID =
        nsearch.add_point_set(positions.front().data(), positions.size());
  } else {
    if (bufferSize != positions.size())
      positions.resize(bufferSize);

    for (int i = 0; i < bufferSize; ++i) {
      const auto element = (*buffer)[i];
      positions[i] = std::array<CompactNSearch::Real, 3>{
        element.pos.x, element.pos.y, element.pos.z };
    }

    nsearch.resize_point_set(pointSetID, positions.front().data(), bufferSize);
    nsearch.update_point_sets();
  }
}

int
NSearchExt::nPoints() const
{
  return static_cast<int>(pointSet->n_points());
}

int
NSearchExt::nNeighbor(uint index) const
{
  return static_cast<int>(pointSet->n_neighbors(pointSetID, index));
}

int
NSearchExt::neighbor(uint index, uint neighbor_index) const
{
  return static_cast<int>(
      pointSet->neighbor(pointSetID, index, neighbor_index));
}

std::shared_ptr<NSearch>
makeNSearch(float border, float radius)
{
  return std::make_shared<NSearch>(border, radius);
}

std::shared_ptr<NSearch>
makeNSearchExt(float border, float radius)
{
  return std::make_shared<NSearchExt>(border, radius);
}

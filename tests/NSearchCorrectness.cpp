#include "NSearch.hpp"
#include "Particle.hpp"
#include "common.hpp"
#include "gtest/gtest.h"

#include <algorithm>
#include <random>
#include <set>

constexpr int NParticles = 2000;

TEST(NSearch, main)
{
  float radius = 1.0f;
  float border = 5.0f;
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<float> dist(-border, border);

  NSearchExt nsearch(border, radius);
  auto buffer = makePBuffer();
  for (int i = 0; i < NParticles; ++i)
    buffer->push_back(SPHParticle(dist(mt), dist(mt), dist(mt)));

  const auto &data = *buffer;
  nsearch.setBuffer(buffer);
  nsearch.build();

  for (uint i = 0; i < data.size(); ++i) {
    uint cnt = 0;
    for (uint j = 0; j < data.size(); ++j)
      if (data[i].dist(data[j]) <= radius) ++cnt;
    EXPECT_EQ(cnt - 1, nsearch.nNeighbor(i));
  }
}

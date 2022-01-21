//
// Created by kr2 on 12/24/21.
//

#include "common.hpp"

#include <filesystem>

[[maybe_unused]] CUDA_FUNC_DEC float
fastPow(float a, int b)
{
  float res = 1.0f;
  for (; b; b >>= 1) {
    if (b & 1) res *= a;
    a *= a;
  }
  return res;
}

[[maybe_unused]] CUDA_FUNC_DEC vec3
colorRamp(float t, const color &col_left, const color &col_right)
{
  return (1 - t) * col_left + t * col_right;
}

std::string
getPath(const std::string &target, int depth)
{
  std::string path = target;
  namespace fs = std::filesystem;
  for (int i = 0; i < depth; ++i) {
    if (fs::exists(path)) { return path; }
    path = "../" + path;
  }

  Warn("failed to get file");
  return target;
}

float
unif(float a, float b)
{
  static thread_local std::mt19937 engine{ std::random_device{}() };
  std::uniform_real_distribution<float> dis(a, b);
  return dis(engine);
}

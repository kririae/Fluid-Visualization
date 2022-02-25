//
// Created by kr2 on 12/24/21.
//

#ifndef FLUIDVISUALIZATION_SRC_COMMON_HPP
#define FLUIDVISUALIZATION_SRC_COMMON_HPP

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random>

// For consideration of flexibility
using color = glm::vec3;
using vec3 = glm::vec3;
using ivec3 = glm::ivec3;
#ifdef _WIN32
using uint = glm::uint;
using ulong = long;
#endif

template <typename T>
using hvector = std::vector<T>;

using namespace std::literals::string_literals;

// #define CUDA_FUNC_DEC __attribute__((host)) __attribute__((device))
#define CUDA_FUNC_DEC 

#define TODO()                                                  \
  do {                                                          \
    std::cerr << __FILE__ << " " << __LINE__ << " " << __func__ \
              << " is not implemented" << std::endl;            \
  } while (0)

#define Assert(cond, ...)           \
  do {                              \
    if (!(cond)) {                  \
      fflush(stdout);               \
      fprintf(stderr, "\33[1;31m"); \
      fprintf(stderr, __VA_ARGS__); \
      fprintf(stderr, "\33[0m\n");  \
      assert(cond);                 \
    }                               \
  } while (0)

#define Warn(...)               \
  fflush(stdout);               \
  fprintf(stderr, "\33[1;31m"); \
  fprintf(stderr, __VA_ARGS__); \
  fprintf(stderr, "\33[0m\n")

#define CUDA_CALL(x)                                  \
  do {                                                \
    if ((x) != cudaSuccess) {                         \
      printf("Error at %s:%d\n", __FILE__, __LINE__); \
    }                                                 \
  } while (0)

float unif(float a = 0.0f, float b = 1.0f);

// Function definition
CUDA_FUNC_DEC float fastPow(float a, int b);
CUDA_FUNC_DEC vec3 colorRamp(float t, const color &col_left,
                             const color &col_right);

std::string getPath(const std::string &target, int depth = 5);

#endif //FLUIDVISUALIZATION_SRC_COMMON_HPP

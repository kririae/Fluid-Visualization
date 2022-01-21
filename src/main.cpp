#include "GUI.hpp"
#include "IncludeSolvers.h"
#include "Particle.hpp"
#include "Scene.hpp" // as JSON parser
#include "Solver.hpp"

#include <getopt.h>

typedef unsigned int uint;

int
main(int argc, char *argv[])
{
  int opt;
  while ((opt = getopt(argc, argv, "s:"))) {
    switch (opt) {
    case 's': {
      Scene s(optarg);
      exit(EXIT_SUCCESS);
    }

    default:
      fprintf(stderr, "Usage: %s -s SceneFilePath", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  return 0;
}
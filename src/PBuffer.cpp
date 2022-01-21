//
// Created by kr2 on 12/24/21.
//

#include "PBuffer.hpp"

PBuffer makePBuffer() {
  return std::make_shared<std::vector<SPHParticle>>();
}

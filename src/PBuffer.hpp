//
// Created by kr2 on 12/24/21.
//

#ifndef FLUIDVISUALIZATION_SRC_PBUFFER_HPP
#define FLUIDVISUALIZATION_SRC_PBUFFER_HPP

/*
 * PBuffer:
 *   Wrapper for vector to store SPHParticles,
 *   to avoid copying
 */
#include <vector>
#include <memory>
#include "Particle.hpp"

using PBuffer = std::shared_ptr<std::vector<SPHParticle> >;

/*
class PBuffer {
  // space to store SPHParticle Array Buffer
  PBuffer() = default;
  PBuffer(int size);
  PBuffer(const std::vector<SPHParticle> &buffer);
};
*/

PBuffer makePBuffer();

#endif //FLUIDVISUALIZATION_SRC_PBUFFER_HPP

//
// Created by kr2 on 12/24/21.
//

#include "Solver.hpp"
#include "GUI.hpp"
#include "common.hpp"

Solver::Solver(float _border, float _radius, float _epsilon)
    : border(_border), radius(_radius), radius2(_radius * _radius),
      epsilon(_epsilon)
{
}

float
Solver::getBorder()
{
  return border;
}

float
Solver::getRadius()
{
  return radius;
}

ExampleSolver::ExampleSolver(float _border, float _radius, float _epsilon)
    : Solver(_border, _radius, _epsilon)
{
  // Do nothing
  buffer = makePBuffer();
}

/*
 * Call the function `setParticles` for class RTGUI
 */
void
ExampleSolver::updateGUI(RTGUI *gui_ptr)
{
  for (int i = 0; i < n_substeps; ++i)
    this->subStep();
  gui_ptr->setParticles(buffer);
}

/*
 * Only perform particle-wise operation here to
 * maintain the completeness
 */
void
ExampleSolver::addParticle(const SPHParticle &p)
{
  assert(buffer != nullptr);
  buffer->push_back(p);
}

void
ExampleSolver::subStep()
{
  // Do nothing
}

std::shared_ptr<Solver>
makeExampleSolver(float border, float radius, float epsilon)
{
  return static_cast<const std::shared_ptr<Solver> &>(
      std::make_shared<ExampleSolver>(border, radius, epsilon));
}

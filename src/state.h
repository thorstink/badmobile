#pragma once

#include <Eigen/Dense>

struct State {
  Eigen::Vector3d position;
};

/**
 * @brief Fake function to test coverage.
 *
 * @param state
 * @return State
 */
State update(State state) { return state; }
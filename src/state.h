#pragma once

#include <Eigen/Dense>

struct State {
  const std::string name;
  Eigen::Vector2d cmd_vel;
};

/**
 * @brief Fake function to test coverage.
 *
 * @param state
 * @return State
 */
State update(State state) { return state; }
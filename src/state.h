#pragma once

#include <Eigen/Dense>
#include <chrono>
#include <functional>
#include <string>

struct State {
  std::string name;
  std::chrono::steady_clock::time_point timestamp;
};

/**
 * @brief Fake function to test coverage.
 *
 * @param state
 * @return State
 */
State update(State state) { return state; }

using Reducer = std::function<State(State &)>;
auto noop = Reducer([](State &m) { return std::move(m); });
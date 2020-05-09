#pragma once

#include "nlohmann/json.hpp"
#include <Eigen/Dense>
#include <chrono>
#include <functional>
#include <string>

struct State {
  nlohmann::json settings;
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
using Effect = std::function<void()>;
auto noop = Reducer([](State &m) { return std::move(m); });
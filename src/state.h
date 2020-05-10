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

namespace robot {
// silly funtion to test valid config. Is not complete.
inline auto robotConfigValid(const nlohmann::json &config) {
  const auto &name_config = config["robot"];
  const bool name = name_config.contains("name");
  const auto &diff_drive_config =
      config["robot"]["differential_drive_parameters"];
  const bool diffdrive = config.contains("track") &&
                         config.contains("wheel_diameter_left") &&
                         config.contains("wheel_diameter_right");

  const bool hardware = config["robot"].contains("hardware");

  return hardware && name && diffdrive;
}
} // namespace robot
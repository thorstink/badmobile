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
  try {
    const bool name = config["robot"].contains("name");
    const auto &diff_drive_config =
        config["robot"]["parameters"];
    const bool diffdrive = diff_drive_config.contains("track") &&
                           diff_drive_config.contains("wheel_diameter_left") &&
                           diff_drive_config.contains("wheel_diameter_right");

    const auto &hardware_config = config["robot"]["hardware"];

    const bool hardware = config["robot"].contains("hardware");
    return hardware && name && diffdrive;
  } catch (...) {
    return false;
  }
}
} // namespace robot
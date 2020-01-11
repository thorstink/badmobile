#include <catch2/catch.hpp>

#include "state.h"
#include <gtsam/base/Vector.h>
#include <gtsam/geometry/Pose3.h>

TEST_CASE("And there was state") {
  auto pose = gtsam::Pose3();
  auto twist2d = gtsam::Vector2();
  auto state = State{twist2d, pose.translation()};
  REQUIRE(state.position[0] == 0.0);
}

TEST_CASE("And state was used") {
  auto pose = gtsam::Pose3();
  auto twist2d = gtsam::Vector2();
  auto state = State{twist2d, pose.translation()};
  auto new_state = update(state);
  REQUIRE(new_state.position.isApprox(state.position));
}
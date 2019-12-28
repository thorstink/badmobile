#include <catch2/catch.hpp>

#include "state.h"

TEST_CASE("And there was state") {
    auto state = State{Eigen::Vector3d(0.0,0.0,0.0)};
    REQUIRE(state.position[0] == 0.0);
}
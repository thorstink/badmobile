#include "imu_msg.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <rxcpp/rx.hpp>
/*
struct DLPF {
  f(int fs, int fc) : j(imu_t{}), fs_(fs), fc_(fc) {}
  imu_t &operator()(imu_t &s) {
    if (j == 0)
      j = s;
    else
      j = 0.8 * j + 0.2 * s;
    return j;
  };
  imu_t v;
  int fs_;
  int fc_;
};
 */
TEST_CASE("a low pass") {
  SECTION("help me") {
    auto values = rxcpp::observable<>::range(1, 5);
    auto values2 = values.scan(
        0.0, [](double s, int t) { return 0.2 * s + 0.8 * double(t); });
    // auto values2 = values.map(f{});

    values2.subscribe([](int sum) { std::cout << sum << std::endl; });
    REQUIRE(true);
  }
}
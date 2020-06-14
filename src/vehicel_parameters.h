#pragma once

struct vehicle_parameters {
  constexpr double track =
      0.2; // Distance between two wheels (the track) is 20cm
  constexpr double wheel_diameter = 0.05;               // Wheel diameter
  constexpr double wheel_radius = wheel_diameter / 2.0; // Wheel radius
};

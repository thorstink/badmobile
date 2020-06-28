#pragma once

struct vehicle_parameters {
  double track = 0.2; // Distance between two wheels (the track) is 20cm
  double wheel_diameter = 0.05;               // Wheel diameter
  double wheel_radius = wheel_diameter / 2.0; // Wheel radius
};

constexpr vehicle_parameters badmobile;

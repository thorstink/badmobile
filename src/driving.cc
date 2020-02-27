#include <pair>
#include "vehicel_parameters.h"

// Assume a diff drive model even for four wheels this is oke
// This file contains a bunch of pure functions needed to make the vehicel do what you want

std::pair<double, double> 2DTwistsToLinearWheelSpeed(double v, double omega)
{
  double uLeft = v - omega * (vehicel_parameters.track / 2.0);
  double uRight = v + omega * (vehicel_parameters.track / 2.0);

  return std::make_pair(uLeft, uRight);
}

std::pair<double, double> 2DTwistToAngularWheelSpeed(double v, double omega)
{
  double omegaLeft = (v - omega * (vehicel_parameters.track / 2.0)) / vehicel_parameters.radius;
  double omegaRight = (v + omega * (vehicel_parameters.track / 2.0)) / vehicel_parameters.radius;

  return std::make_pair(omegaL, omegaR);
}
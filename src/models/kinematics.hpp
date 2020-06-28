#pragma once

#include <gtsam/base/Vector.h>
#include <gtsam/nonlinear/expressions.h>

double yawRate(const gtsam::Vector3 &v,
               gtsam::OptionalJacobian<1, 3> H = boost::none) {
  if (H) {
    *H << 0.0, 0.0, 1.0;
  }

  return v(2);
}

double vX(const gtsam::Vector3 &v,
          gtsam::OptionalJacobian<1, 3> H = boost::none) {
  if (H) {
    *H << 1.0, 0.0, 0.0;
  }

  return v(0);
}

double vY(const gtsam::Vector3 &v,
          gtsam::OptionalJacobian<1, 3> H = boost::none) {
  if (H) {
    *H << 0.0, 1.0, 0.0;
  }

  return v(1);
}

/**
 * @brief forward kinematic diffdrive
 *
 * @param u
 * @param H
 * @return Vector2
 */
gtsam::Vector3 fk(const gtsam::Vector2 &u, const gtsam::Vector2 &m,
                  gtsam::OptionalJacobian<3, 2> H1 = boost::none,
                  gtsam::OptionalJacobian<3, 2> H2 = boost::none) {
  double u_left = u(0);
  double u_right = u(1);
  double r = m(0);
  double d = m(1);

  gtsam::Vector3 twist;
  twist << r * (u_right + u_left) / 2.0, 0.0, r * (u_right - u_left) / d;

  if (H1) {
    *H1 << r / 2.0, r / 2.0, //
        0.0, 0.0,            //
        r / d, -r / d;
  }

  if (H2) {
    *H2 << (u_right + u_left) / 2.0, 0.0, //
        0.0, 0.0,                         //
        (u_right - u_left) / d, r * (u_left - u_right) / (d * d);
  }

  return twist;
};

/**
 * @brief inverse kinematics diffdrive
 *
 */
gtsam::Vector2 ik(const gtsam::Vector3 &t, const gtsam::Vector2 &m,
                  gtsam::OptionalJacobian<2, 3> H1 = boost::none,
                  gtsam::OptionalJacobian<2, 2> H2 = boost::none) {
  double v_x = t(0);
  double omega_z = t(2);
  double r = m(0);
  double d = m(1);
  double hd = d / 2.0;
  gtsam::Vector2 wheels;
  wheels << (v_x - (omega_z * hd)) / r, (v_x + (omega_z * hd)) / r;

  if (H1) {
    *H1 << 1 / r, 0.0, -hd / r, //
        1 / r, 0.0, hd / r;     //
  }

  double rsq = r * r;
  if (H2) {
    *H2 << -(v_x - (omega_z * hd)) / rsq, -0.5 * omega_z / r, //
        -(v_x + (omega_z * hd)) / rsq, 0.5 * omega_z / r;     //
  }

  return wheels;
};

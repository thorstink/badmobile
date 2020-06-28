#include "models/kinematics.hpp"
#include "nlohmann/json.hpp"
#include "vehicle_parameters.h"

gtsam::Vector2 jsonTwistToVector(const nlohmann::json &json_twist) {
  gtsam::Vector3 twist = gtsam::Vector3(json_twist.at("linear_x"), 0.0,
                                        json_twist.at("angular_z"));
  return ik(twist, gtsam::Vector2(badmobile.wheel_diameter, badmobile.track));
}